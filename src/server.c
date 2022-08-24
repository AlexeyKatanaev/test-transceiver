/**
 * @file server.c
 * @author simohstudent@gmail.com
 * @brief The main module that implements the UDP server on the specified port to process requests to control Transceiver/Amplifier.
 * Server also uses health check mechanism to report their state to stderr.
 * 
 * TODO:
 *  * Implement handling UDP messages
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
#include "healthcheck.h"


struct sCommand
{
    enum {
        COMMAND_AMPLIFIER = 0x0,
        COMMAND_TRANSCEIVER
    } Device;
    int numOfActions;
    int* actions;
};


void PrintHelp()
{
    fprintf(stderr, "\nIncorrect input parammeters\n");
    fprintf(stderr, "Expected parameters:\n");
    fprintf(stderr, "\t-p <port number> - The port number for the udp socket on which signals will be received. [4096-65535]\n");
}

/***
 * Function parse port number from input parameter "-p" <port int value>
 ***/
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

/**
 * @brief Create UDP socket with O_NONBLOCK flag
 * 
 * @param port - socket port
 * @return int :
 * 0 - if success, -1 when failed
 */
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


// void ReadMsgFromSocket(int socketfd)
// {
//     int msg_len = 0;
//     if ((msg_len = recvfrom(socketfd, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
//     {
//         die("recvfrom()");
//     }
// }


int main(int argc, char** argv)
{
    int activePort;
    int socketfd;
    Transceiver transceiver;
    HealthCheck healthcheck;

    activePort = GetPortNumberFromParams(argc, argv);
    if (activePort < 4096 || activePort > 65535)
    {
        PrintHelp();
        return EXIT_FAILURE;
    }

    transceiver = Transceiver_Create();
    healthcheck = HealthCheck_Create(transceiver, NULL, 1); 
    
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
            continue;
        }
        if (ret == 0) continue;

        if (fds[0].revents & POLLIN)
        {
            fds[0].revents = 0;
            // if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
            // {
            //     die("recvfrom()");
            // }
            // fprintf(stderr, "Some msg from socket\n");
        }   
    }

    close(socketfd);
    HealthCheck_Stop(healthcheck);
    free(healthcheck);
    free(transceiver);

    fprintf(stderr, "Stopping server...\n");
    return EXIT_SUCCESS;
}
