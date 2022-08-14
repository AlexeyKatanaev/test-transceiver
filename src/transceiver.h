#pragma once

typedef enum
{
    TRANSCEIVER_STATE_OFF = 0x0,
    TRANSCEIVER_STATE_RX,
    TRANSCEIVER_STATE_TX,
    TRANSCEIVER_STATE_RXTX
} TransceiverState;

typedef struct transceiver_s *Transceiver;

Transceiver Transceiver_Create();
void Transceiver_SetState(Transceiver t, TransceiverState state);
TransceiverState Transceiver_GetState(Transceiver t);
