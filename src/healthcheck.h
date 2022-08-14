/*
    Module which track state of input Transceiver and logging to stderr.
    Interval can be configured using checkIntervalSec parameter.
    Current implementation can only handle single of transciver instance.
*/

#pragma once

#include "transceiver.h"

int Healthcheck_Initialize(Transceiver t, unsigned int checkIntervalSec);
void Healthcheck_Destroy();
