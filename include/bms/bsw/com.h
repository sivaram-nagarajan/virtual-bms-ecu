/**
 * @file com.h
 * @brief Signal packing and unpacking for application frames (dbc/bms.dbc).
 *
 * Skeleton: receive indication only. Signal decoding, timeout monitoring and
 * transmission are added in PR 2 (REQ-CELL-003, REQ-E2E-001).
 */
#ifndef BMS_BSW_COM_H
#define BMS_BSW_COM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void Com_Init(void);
void Com_RxIndication(uint16_t pduId, const uint8_t *data, uint8_t dlc);
void Com_MainFunctionTx(void);
uint32_t Com_GetRxCount(uint16_t pduId);

#ifdef __cplusplus
}
#endif

#endif /* BMS_BSW_COM_H */
