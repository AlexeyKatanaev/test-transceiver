#include <time.h>
#include <stdlib.h>
#include "amplifier.h"

struct sAmplifier
{
    AmplifierState state;
    int lastUpdate;
};


Amplifier Amplifier_Create()
{   
    Amplifier instance = malloc(sizeof(struct sAmplifier));
    instance->state = AMPLIFIER_STATE_OFF;
    instance->lastUpdate = 0;
    return instance;
}


void Amplifier_SetState(Amplifier amp, AmplifierState state)
{
    amp->state = state;
    amp->lastUpdate = (unsigned)time(NULL);
}


AmplifierState Amplifier_GetState(Amplifier amp)
{
    return amp->state;
}
