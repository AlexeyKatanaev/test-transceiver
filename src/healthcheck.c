#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>

#include "healthcheck.h"

timer_t g_timerid;
unsigned int g_timerIntervalSec = 1;

int TriggerHealthcheckTimer()
{
    struct itimerspec trigger;
    memset(&trigger, 0, sizeof(struct itimerspec));
    trigger.it_value.tv_sec = g_timerIntervalSec;
    return timer_settime(g_timerid, 0, &trigger, NULL);
}


void handler(union sigval sv)
{
    Transceiver t = sv.sival_ptr;
    fprintf(stderr, "Transceiver state: %d\n", Transceiver_GetState(t));
    
    if (TriggerHealthcheckTimer() != 0)
    {
        fprintf(stderr, "Cannot retrigger timer: %s\n", strerror(errno));
        // TODO: Smth is wrong. Cannot track server state. Send sigterm and restart app
    }
}


int Healthcheck_Initialize(Transceiver t, unsigned int checkIntervalSec)
{
    assert(!g_timerid); // Redesign g_timerid if we want to track several instances
    fprintf(stderr, "Creating timer to handle object state every %d sec\n", checkIntervalSec);

    struct sigevent sev;
    memset(&sev, 0, sizeof(struct sigevent));

    g_timerIntervalSec = checkIntervalSec;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &handler;
    sev.sigev_value.sival_ptr = t;

    int status = timer_create(CLOCK_REALTIME, &sev, &g_timerid);
    if (status != 0)
    {
        // TODO: Handle EAGAIN and add retry mechanism
        fprintf(stderr, "Cannot create timer: %s\n", strerror(errno));
        return -1;
    }
    if (TriggerHealthcheckTimer() != 0)
    {
        fprintf(stderr, "Cannot initialize timer!\n");
        return -1;
    }
    return 0;
}

void Healthcheck_Destroy()
{
    fprintf(stderr, "Destroying healthcheck timer\n");
    int status = timer_delete(g_timerid);
    if (status != 0)
    {
        fprintf(stderr, "Cannot destroy timer: %s\n", strerror(errno));
    }
}
