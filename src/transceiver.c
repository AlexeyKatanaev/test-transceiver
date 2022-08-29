#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "transceiver.h"

struct sTransceiver
{
    TransceiverState state;
    unsigned int lastUpdate;
};


Transceiver Transceiver_Create()
{   
    Transceiver instance = malloc(sizeof(struct sTransceiver));
    Transceiver_SetState(instance, TRANSCEIVER_STATE_RX);
    return instance;
}


void Transceiver_SetState(Transceiver tcvr, TransceiverState state)
{
    tcvr->state = state;
    tcvr->lastUpdate = (unsigned)time(NULL);
    fprintf(stderr, "[Transceiver] State cnanged to %s\n", Transceiver_StateToStr(state));
}   


TransceiverState Transceiver_GetState(Transceiver tcvr)
{
    return tcvr->state;
}


const char* Transceiver_StateToStr(TransceiverState state)
{
    switch (state)
    {
        case TRANSCEIVER_STATE_RX: return "RX";
        case TRANSCEIVER_STATE_TX: return "TX";
        case TRANSCEIVER_STATE_RXTX: return "RXTX";
        default: return "UNKNOWN";
    }
}