/** @file bms_tasks.h
 *  @brief Task bodies called by the scheduler (runnable order: docs/architecture.md §4). */
#ifndef BMS_ECU_TASKS_H
#define BMS_ECU_TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

void Bms_InitAll(void);
void Bms_Task10ms(void);
void Bms_Task100ms(void);

#ifdef __cplusplus
}
#endif

#endif
