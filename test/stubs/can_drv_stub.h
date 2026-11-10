/** @file can_drv_stub.h
 *  @brief Test double for CanDrv: inject Rx frames and inspect Tx frames. */
#ifndef BMS_TEST_CAN_DRV_STUB_H
#define BMS_TEST_CAN_DRV_STUB_H

#include "bms/platform/can_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAN_STUB_QUEUE_LEN (64U)

void CanDrvStub_Reset(void);
void CanDrvStub_InjectRx(uint32_t id, const uint8_t *data, uint8_t dlc);
uint32_t CanDrvStub_TxCount(void);
const Can_FrameType *CanDrvStub_TxFrame(uint32_t index);

#ifdef __cplusplus
}
#endif

#endif
