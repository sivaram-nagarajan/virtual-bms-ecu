/**
 * @file cantp.h
 * @brief ISO 15765-2 transport protocol for UDS on 0x7E0/0x7E8.
 *
 * Skeleton: receive indication only. Segmentation and flow control follow in
 * PR 3 (REQ-DCM-001).
 */
#ifndef BMS_BSW_CANTP_H
#define BMS_BSW_CANTP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void CanTp_Init(void);
void CanTp_RxIndication(uint16_t pduId, const uint8_t *data, uint8_t dlc);
void CanTp_MainFunction(void);

#ifdef __cplusplus
}
#endif

#endif /* BMS_BSW_CANTP_H */
