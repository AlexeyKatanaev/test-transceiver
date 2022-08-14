#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "transceiver.h"
#include "healthcheck.h"


void printHelp()
{
    fprintf(stderr, "\nIncorrect input parammeters\n");
    fprintf(stderr, "Expected parameters:\n");
    fprintf(stderr, "\t-p <port number> - The port number for the udp socket on which signals will be received.\n");
}

int main(int argc, char** argv)
{
    if(argc != 3 || strcmp(argv[1], "-p") != 0)
    {
        printHelp();
        return -1;
    }
    int activePort = atoi(argv[2]);
    if (activePort < 4096 || activePort > 65535)
    {
        printHelp();
        return -1;
    }

    fprintf(stderr, "Starting UDP server on port %d\n", 31337);
    unsigned int updateInterval = 1; // Seconds
    Transceiver transceiver = Transceiver_Create();

    if (Healthcheck_Initialize(transceiver, updateInterval) != 0)
    {
        fprintf(stderr, "Server starting failed! Cannot initialize healthcheck. Try to restart...\n");
        return -1;
    }

    // TODO: place for main loop
    sleep(5);
    Transceiver_SetState(transceiver, TRANSCEIVER_STATE_RX);
    sleep(5);

    Healthcheck_Destroy();
    fprintf(stderr, "Stopping server...\n");
    return 0;
}
