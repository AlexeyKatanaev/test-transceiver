/**
 * @file amplifier.h
 * @author simohstudent@gmail.com
 * @brief Module that implements access to Amplifier and changing its state..
 * It uses FSM to control the amplifier.
 * Theoretically, you can create multiple amplifier entities for different devices.
 * The amplifier can be turned on or off. Free up memory after work
 * NOTE: Right now it's a dummy module which log state to stderr
 * @version 0.1
 * @date 2022-08-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

typedef enum
{
    AMPLIFIER_STATE_OFF = 0x0,
    AMPLIFIER_STATE_ON
} AmplifierState;

typedef struct sAmplifier *Amplifier;

/**
 * @brief Function which initialize Amplifier FSM
 * 
 * @return Amplifier - pointer to Amplifier state structure
 */
Amplifier Amplifier_Create();

/**
 * @brief Function which change amplifier state
 * 
 * @param amp - pointer to amplifier state structure
 * @param state - new amplifier state
 */
void Amplifier_SetState(Amplifier amp, AmplifierState state);
/**
 * @brief Function to get amplifier current state
 * 
 * @param amp - pointer to amplifier state structure
 * @return AmplifierState 
 */
AmplifierState Amplifier_GetState(Amplifier amp);
