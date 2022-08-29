#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "amplifier.h"

struct sAmplifier
{
    AmplifierState state;
    int lastUpdate;
};


Amplifier Amplifier_Create()
{   
    Amplifier instance = malloc(sizeof(struct sAmplifier));
    Amplifier_SetState(instance, AMPLIFIER_STATE_OFF);
    return instance;
}


void Amplifier_SetState(Amplifier amp, AmplifierState state)
{
    amp->state = state;
    amp->lastUpdate = (unsigned)time(NULL);
    fprintf(stderr, "[Amplifier] State cnanged to %s\n", Amplifier_StateToStr(state));
}


AmplifierState Amplifier_GetState(Amplifier amp)
{
    return amp->state;
}

const char* Amplifier_StateToStr(AmplifierState state)
{
    switch (state)
    {
        case AMPLIFIER_STATE_OFF: return "OFF";
        case AMPLIFIER_STATE_ON: return "ON";
        default: return "UNKNOWN";
    }
}
