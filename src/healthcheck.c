#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include "healthcheck.h"

struct sHealthCheck
{
    Transceiver tcvr;
    Amplifier amp;
    timer_t timerid;
    unsigned int checkIntervalSec;
};


int TriggerHealthCheckTimer(HealthCheck healthcheck)
{
    assert(healthcheck);
    
    struct itimerspec trigger;

    if (!healthcheck->tcvr && !healthcheck->amp)
    {
        fprintf(stderr, "Transceiver and Amplifier was not specified. No reasons to create health checker\n");
        return -1;
    }
    
    memset(&trigger, 0, sizeof(struct itimerspec));
    trigger.it_value.tv_sec = healthcheck->checkIntervalSec;
    return timer_settime(healthcheck->timerid, 0, &trigger, NULL);
}


void HealthCheckHandler(union sigval sv)
{
    HealthCheck healthcheck = sv.sival_ptr;
    if (healthcheck->tcvr)
    {
        fprintf(stderr, "Transceiver state: %d\n", Transceiver_GetState(healthcheck->tcvr));
    }
    if (healthcheck->amp)
    {
        fprintf(stderr, "Amplifier state: %d\n", Amplifier_GetState(healthcheck->amp));
    }
    
    if (TriggerHealthCheckTimer(healthcheck) != 0)
    {
        fprintf(stderr, "Cannot retrigger timer: %s\n", strerror(errno));
        // TODO: Smth is wrong. Cannot track server state. Send sigterm and restart app
    }
}

HealthCheck HealthCheck_Create(Transceiver tcvr, Amplifier amp, unsigned int checkIntervalSec)
{
    HealthCheck instance = malloc(sizeof(struct sHealthCheck));
    instance->tcvr = tcvr;
    instance->amp = amp;
    instance->checkIntervalSec = checkIntervalSec;
    instance->timerid = 0;
    return instance;
}

int HealthCheck_Start(HealthCheck healthcheck)
{
    assert(healthcheck);

    struct sigevent sev;
    int ret;

    fprintf(stderr, "Creating timer to handle object state every %d sec\n", healthcheck->checkIntervalSec);

    memset(&sev, 0, sizeof(struct sigevent));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &HealthCheckHandler;
    sev.sigev_value.sival_ptr = healthcheck;
    ret = timer_create(CLOCK_REALTIME, &sev, &healthcheck->timerid);
    if (ret != 0)
    {
        // TODO: Handle EAGAIN and add retry mechanism
        fprintf(stderr, "Cannot create timer: %s\n", strerror(errno));
        return -1;
    }
    if (TriggerHealthCheckTimer(healthcheck) != 0)
    {
        fprintf(stderr, "Cannot initialize timer!\n");
        return -1;
    }
    return 0;
}

void HealthCheck_Stop(HealthCheck healthcheck)
{
    assert(healthcheck);
    fprintf(stderr, "Destroying healthcheck timer\n");
    int ret = timer_delete(healthcheck->timerid);
    if (ret != 0)
    {
        fprintf(stderr, "Cannot destroy timer: %s\n", strerror(errno));
    }
}
