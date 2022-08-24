/**
 * @file transceiver.h
 * @author simohstudent@gmail.com
 * @brief  Module that implements access to Transceiver and changing its state.
 * It uses FSM to control the amplifier. 
 * You can create multiple such entities for each device and manage them independently.
 * @version 0.1
 * @date 2022-08-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

typedef enum
{
    TRANSCEIVER_STATE_OFF = 0x0,
    TRANSCEIVER_STATE_RX,
    TRANSCEIVER_STATE_TX,
    TRANSCEIVER_STATE_RXTX
} TransceiverState;

typedef struct sTransceiver *Transceiver;

/**
 * @brief Initialing and create Transceiver state entity
 * Default state is OFF
 * @return Transceiver - pointer to Transciever state
 */
Transceiver Transceiver_Create();
/**
 * @brief Change state for specific transceiver entity
 * 
 * @param tcvr - pointer to Transceiver state
 * @param state - new Transceiver state
 */
void Transceiver_SetState(Transceiver tcvr, TransceiverState state);
/**
 * @brief Return Transceiver instance state
 * 
 * @param tcvr - pointer to Transciver state
 * @return TransceiverState 
 */
TransceiverState Transceiver_GetState(Transceiver tcvr);
