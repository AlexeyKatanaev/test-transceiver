/**
 * @file server.c
 * @author simohstudent@gmail.com
 * @brief The main module that implements the UDP server on the specified port to process requests to control Transceiver/Amplifier.
 * Server also uses health check mechanism to report their state to stderr.
 * 
 * @version 0.1
 * @date 2022-08-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include "transceiver.h"
#include "amplifier.h"
#include "healthcheck.h"


typedef enum
{
    DEVICE_AMPLIFIER = 0x0,
    DEVICE_TRANSCEIVER,

    DEVICE_UNKNOWN = 0xFF

} DeviceTag;

static void PrintHelp();
/***
 * Function parse port number from input parameter "-p" <port int value>
 ***/
static int GetPortNumberFromParams(int argc, char** argv);
/**
 * @brief Create UDP socket with O_NONBLOCK flag
 * 
 * @param port - socket port
 * @return int :
 * 0 - if success, -1 when failed
 */
static int CreateSocket(int port);
/**
 * @brief Function extracting TLV message from input socket, aplying commands and send input message back
 * 
 * @param socketfd 
 */
static void HandleSocketMsg(int socketfd, Transceiver tcvr, Amplifier amp);


int main(int argc, char** argv)
{
    int activePort;
    int socketfd;
    Transceiver transceiver;
    Amplifier amplifier;
    HealthCheck healthcheck;

    activePort = GetPortNumberFromParams(argc, argv);
    if (activePort < 4096 || activePort > 65535)
    {
        PrintHelp();
        return EXIT_FAILURE;
    }

    transceiver = Transceiver_Create();
    amplifier = Amplifier_Create();
    healthcheck = HealthCheck_Create(transceiver, amplifier, 1); 
    
    if (HealthCheck_Start(healthcheck) != 0)
    {
        fprintf(stderr, "Server starting failed! Cannot initialize healthcheck. Try to restart...\n");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Creating UDP server on port %d\n", activePort);
    if ((socketfd = CreateSocket(activePort)) < 0)
    {
        fprintf(stderr, "Cannot create socket: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    while(1)
    {
        struct pollfd fds[1];
        fds[0].fd = socketfd;
        fds[0].events = POLLIN;

        int ret = poll(fds, 1, -1);
        if ( ret == -1 )
        {
            fprintf(stderr, "Failed to execute poll operation: %s\n", strerror(errno));
            continue;  // error counter + break?
        }

        if (fds[0].revents & POLLIN)
        {
            HandleSocketMsg(socketfd, transceiver, amplifier);
        }   
    }

    fprintf(stderr, "Stopping server...\n");
    close(socketfd);
    HealthCheck_Stop(healthcheck);

    return EXIT_SUCCESS;
}


void PrintHelp()
{
    fprintf(stderr, "\nIncorrect input parammeters\n");
    fprintf(stderr, "Expected parameters:\n");
    fprintf(stderr, "\t-p <port number> - The port number for the udp socket on which signals will be received. [4096-65535]\n");
}


int GetPortNumberFromParams(int argc, char** argv)
{
    for(int idx = 0; idx < argc; idx++)
    {
        if (strcmp(argv[idx], "-p") != 0) continue;
        if (idx > argc - 1) break;
        int port = atoi(argv[idx+1]);
        return port;
    }
    return -1;
}


int CreateSocket(int port)
{
    int socketfd;
    int socketFlags;
    struct sockaddr_in servaddr;

    if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

    if (bind(socketfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(stderr, "Failed to bind socket: %s\n", strerror(errno));
        return -1;
    }

    socketFlags = fcntl(socketfd, F_GETFL, 0);
    if (socketFlags == -1)
    {
        fprintf(stderr, "Failed to get socket flags: %s\n", strerror(errno));
        return -1;
    }
    socketFlags |= O_NONBLOCK;
    if (fcntl(socketfd, F_SETFL, socketFlags) != 0)
    {
        fprintf(stderr, "Failed to setup O_NONBLOCK flag for socket: %s\n", strerror(errno));
        return -1;
    }

    return socketfd;
}

void HandleSocketMsg(int socketfd, Transceiver tcvr, Amplifier amp)
{
    #define BUFSIZE 1024
    int msgLen = 0;
    DeviceTag deviceTag = DEVICE_UNKNOWN;
    int commandsCount = 0;
    char buf[BUFSIZE];
    struct sockaddr fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);

    // Receive TLV message
    msgLen = recvfrom(socketfd, buf, BUFSIZE, 0, &fromAddr, &fromAddrLen);
    if (msgLen == -1)
    {
        fprintf(stderr, "Cannot read message from socket! %s\n", strerror(errno));
        return;
    }
    if (msgLen < 3)
    {
        fprintf(stderr, "Incorrect message length! %d", msgLen);
        return;
    }

    // Parse and validate TLV message
    // +-------------+----------------+-------------------------------+
    // |  0-1 byte   |    2-3 byte    |          4-... byte           |
    // +-------------+----------------+-------------------------------+
    // | Device Tag  | Commands Count | Commands (1 byte per command) |
    // +-------------+----------------+-------------------------------+
    // Tag: 2 bytes
    switch ((int)buf[0] << 4 | buf[1])
    {
        case 0x00: deviceTag = DEVICE_AMPLIFIER; break;
        case 0x01: deviceTag = DEVICE_TRANSCEIVER; break;
        default:
            fprintf(stderr, "Unknown Tag! Skiping message...\n");
            return;
    }

    // Length: 2 bytes
    commandsCount = (int)buf[2] << 4 | buf[3];
    if (commandsCount < 1 || commandsCount != msgLen - 4)
    {
        fprintf(stderr, "Length is incorrect! Skipping message...\n");
        return;
    }

    // Value: Commands in the buffer start from the 5th byte
    for (int cmdIdx = 0; cmdIdx < commandsCount; cmdIdx++)
    {
        int command = (int)buf[4 + cmdIdx];
        switch (deviceTag)
        {
        case DEVICE_AMPLIFIER:
            if (command < AMPLIFIER_STATE_OFF || command > AMPLIFIER_STATE_ON)
            {
                fprintf(stderr, "Incorrect command (%d) for Amplifier by index: %d. Skipping...\n", command, cmdIdx);
                continue;
            }
            Amplifier_SetState(amp, command);
            break;
        case DEVICE_TRANSCEIVER:
            if (command < TRANSCEIVER_STATE_RX || command > TRANSCEIVER_STATE_RXTX)
            {
                fprintf(stderr, "Incorrect command (%d) for Transceiver by index: %d. Skipping...\n", command, cmdIdx);
                continue;
            }
            Transceiver_SetState(tcvr, command);
            break;
        default:
            break;
        }
    }

    // Send original message back
    if (sendto(socketfd, buf, msgLen, MSG_DONTWAIT, &fromAddr, fromAddrLen) == -1)
    {
        fprintf(stderr, "Cannot send response to destination: %s! %s\n", fromAddr.sa_data, strerror(errno));   
    }
}
