/**
 * @file os_linux.c
 * @brief Linux main loop for the cyclic scheduler (CLOCK_MONOTONIC, absolute sleeps).
 */

#include <errno.h>
#include <signal.h>
#include <time.h>

#include "bms/platform/os.h"

#define NS_PER_MS (1000000L)
#define NS_PER_S (1000000000L)
#define MS_PER_S (1000U)

static volatile sig_atomic_t os_shutdownRequested = 0;

static void os_addMs(struct timespec *ts, long ms)
{
    ts->tv_nsec += ms * NS_PER_MS;
    while (ts->tv_nsec >= NS_PER_S) {
        ts->tv_nsec -= NS_PER_S;
        ts->tv_sec++;
    }
}

uint32_t Os_GetTimeMs(void)
{
    struct timespec now;
    (void)clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint32_t)(((uint64_t)now.tv_sec * MS_PER_S) +
                      ((uint64_t)now.tv_nsec / (uint64_t)NS_PER_MS));
}

void Os_RequestShutdown(void)
{
    os_shutdownRequested = 1;
}

void Os_Run(void)
{
    struct timespec next;
    (void)clock_gettime(CLOCK_MONOTONIC, &next);

    while (os_shutdownRequested == 0) {
        Os_Tick(Os_GetTimeMs());
        os_addMs(&next, (long)OS_TICK_MS);
        while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL) == EINTR) {
            if (os_shutdownRequested != 0) {
                break;
            }
        }
    }
}
