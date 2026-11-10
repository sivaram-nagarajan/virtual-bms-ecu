/**
 * @file os.c
 * @brief Platform-independent part of the cyclic scheduler.
 */
#include "bms/platform/os.h"

#include <stddef.h>

static const Os_ConfigType *os_config = NULL;
static uint32_t os_tickCount = 0U;
static uint32_t os_overrunCount = 0U;
static uint32_t os_nextReleaseMs = 0U;

void Os_Init(const Os_ConfigType *config, uint32_t startTimeMs)
{
    os_config = config;
    os_tickCount = 0U;
    os_overrunCount = 0U;
    os_nextReleaseMs = startTimeMs;
}

void Os_Tick(uint32_t nowMs)
{
    /* Unsigned subtraction handles wrap-around of the millisecond counter. */
    const uint32_t lateness = nowMs - os_nextReleaseMs;
    if ((lateness < (UINT32_MAX / 2U)) && (lateness >= OS_TICK_MS)) {
        os_overrunCount++;
        /* Re-synchronise instead of running a burst of catch-up ticks. */
        os_nextReleaseMs = nowMs;
    }
    os_nextReleaseMs += OS_TICK_MS;

    if (os_config != NULL) {
        if (os_config->task10ms != NULL) {
            os_config->task10ms();
        }
        if (((os_tickCount % OS_SLOW_TASK_DIVIDER) == 0U) && (os_config->task100ms != NULL)) {
            os_config->task100ms();
        }
    }
    os_tickCount++;
}

uint32_t Os_GetTickCount(void)
{
    return os_tickCount;
}

uint32_t Os_GetOverrunCount(void)
{
    return os_overrunCount;
}
