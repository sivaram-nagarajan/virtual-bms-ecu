/**
 * @file bms_tasks.c
 * @brief Task bodies. Runnables that are not implemented yet are listed as
 *        comments in their final position so the order is visible now.
 */
#include "bms_tasks.h"

#include "bms/bsw/canif.h"
#include "bms/bsw/cantp.h"
#include "bms/bsw/com.h"
#include "ecu_cfg.h"

void Bms_InitAll(void)
{
    CanIf_Init(&Ecu_CanIfConfig);
    Com_Init();
    CanTp_Init();
}

void Bms_Task10ms(void)
{
    CanIf_MainFunctionRx();
    CanTp_MainFunction();
    /* Dcm_MainFunction();          PR 3 */
    /* Protection_Run10ms();        PR 2 */
    /* ContactorCtrl_Run10ms();     PR 2 */
    Com_MainFunctionTx();
}

void Bms_Task100ms(void)
{
    /* CellMonitor_Run100ms();      PR 2 */
    /* SocEstimator_Run100ms();     PR 2 */
    /* Dem_MainFunction();          PR 3 */
}
