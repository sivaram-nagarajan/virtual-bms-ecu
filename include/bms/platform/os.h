/**
 * @file os.h
 * @brief Time-triggered cyclic scheduler (platform layer, replaces AUTOSAR Os).
 *
 * Os_Tick() holds the scheduling logic and does not depend on Linux, so it is
 * unit-tested directly. Os_Run() (src/platform/os_linux.c) calls it every
 * 10 ms using CLOCK_MONOTONIC and absolute sleeps, so there is no drift.
 *
 * @satisfies REQ-OS-001, REQ-OS-002
 */
#ifndef BMS_PLATFORM_OS_H
#define BMS_PLATFORM_OS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_TICK_MS (10U)
#define OS_SLOW_TASK_DIVIDER (10U) /**< 100 ms task runs every 10th tick. */

typedef void (*Os_TaskFnType)(void);

/** Task bodies. A struct, so the two tasks cannot be swapped by mistake. */
typedef struct {
    Os_TaskFnType task10ms;
    Os_TaskFnType task100ms;
} Os_ConfigType;

/** Registers the task bodies and resets counters. */
void Os_Init(const Os_ConfigType *config, uint32_t startTimeMs);

/**
 * Runs one scheduler tick.
 * @param nowMs Monotonic time at which this tick actually started.
 */
void Os_Tick(uint32_t nowMs);

uint32_t Os_GetTickCount(void);
uint32_t Os_GetOverrunCount(void);

/** Blocking main loop (Linux). Returns after Os_RequestShutdown(). */
void Os_Run(void);

/** Async-signal-safe: ends Os_Run() after the current tick. */
void Os_RequestShutdown(void);

/** Monotonic milliseconds since an arbitrary point. */
uint32_t Os_GetTimeMs(void);

#ifdef __cplusplus
}
#endif

#endif /* BMS_PLATFORM_OS_H */
