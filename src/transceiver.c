#include <time.h>
#include <stdlib.h>
#include "transceiver.h"

struct sTransceiver
{
    TransceiverState state;
    unsigned int lastUpdate;
};


Transceiver Transceiver_Create()
{   
    Transceiver instance = malloc(sizeof(struct sTransceiver));
    instance->state = TRANSCEIVER_STATE_OFF;
    instance->lastUpdate = 0;
    return instance;
}


void Transceiver_SetState(Transceiver tcvr, TransceiverState state)
{
    tcvr->state = state;
    tcvr->lastUpdate = (unsigned)time(NULL);
}


TransceiverState Transceiver_GetState(Transceiver tcvr)
{
    return tcvr->state;
}
