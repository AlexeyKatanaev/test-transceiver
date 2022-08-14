#include <time.h>
#include <stdlib.h>
#include "transceiver.h"

struct transceiver_s
{
    TransceiverState state;
    unsigned int last_update;
};


Transceiver Transceiver_Create()
{   
    Transceiver instance = malloc(sizeof(struct transceiver_s));
    instance->state = TRANSCEIVER_STATE_OFF;
    instance->last_update = 0;
    return instance;
}


void Transceiver_SetState(Transceiver t, TransceiverState state)
{
    t->state = state;
    t->last_update = (unsigned)time(NULL);
}


TransceiverState Transceiver_GetState(Transceiver t)
{
    return t->state;
}
