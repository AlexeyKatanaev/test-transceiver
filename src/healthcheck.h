/**
 * @file healthcheck.h
 * @author simohstudent@gmail.com
 * @brief Module to track Transceiver/Amplifier state
 * It uses timer which triggered in separated thread.
 * You can create multiple instances. One instance can handle one Transceiver/Amplifier instance (or both)
 * @version 0.1
 * @date 2022-08-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "transceiver.h"
#include "amplifier.h"


typedef struct sHealthCheck *HealthCheck;

/**
 * @brief Function creates HealthCheck structure to handle input objects with specified interval
 * At least a one pointer must be specified
 * @param tcvr - pointer to Transciever [can be NULL]
 * @param amp - pointer to Amplifier [can be NULL]
 * @param checkIntervalSec - interval for health checker in seconds
 * @return pointer to HealthCheck
 */
HealthCheck HealthCheck_Create(Transceiver tcvr, Amplifier amp, unsigned int checkIntervalSec);
/**
 * @brief Starting health check procedure for specifier HealthCheck instance
 * 
 * @param healthcheck - pointer to HealthCheck structure
 * @return int :
 *  0 - if success, -1 when failed
 */
int HealthCheck_Start(HealthCheck healthcheck);
/**
 * @brief Function stops healthCheck timer
 * 
 * @param healthcheck - pointer to HealthCheck structure
 */
void HealthCheck_Stop(HealthCheck healthcheck);
