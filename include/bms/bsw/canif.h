/**
 * @file canif.h
 * @brief CAN interface: routes received frames to upper layers by CAN ID.
 *
 * Routing is table-driven (see CanIf_ConfigType), the same idea as the
 * AUTOSAR CanIf Rx PDU configuration without a generator.
 *
 * @satisfies REQ-CANIF-001, REQ-CANIF-002
 */
#ifndef BMS_BSW_CANIF_H
#define BMS_BSW_CANIF_H

#include <stdint.h>

#include "bms/platform/can_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum frames processed per CanIf_MainFunctionRx() call (bounds runtime). */
#define CANIF_MAX_RX_PER_CYCLE (32U)

/** Upper-layer receive callback (e.g. Com_RxIndication, CanTp_RxIndication). */
typedef void (*CanIf_RxIndicationFnType)(uint16_t pduId, const uint8_t *data, uint8_t dlc);

typedef struct {
    uint32_t canId;
    uint8_t minDlc;                         /**< Shorter frames are discarded. */
    uint16_t pduId;                         /**< Handle passed to the upper layer. */
    CanIf_RxIndicationFnType rxIndication;
} CanIf_RxPduConfigType;

typedef struct {
    const CanIf_RxPduConfigType *rxPdus;
    uint16_t numRxPdus;
} CanIf_ConfigType;

typedef struct {
    uint32_t routed;
    uint32_t unknownId;  /**< Frames with a CAN ID not in the table. */
    uint32_t badDlc;     /**< Frames shorter than the configured minimum. */
} CanIf_StatisticsType;

void CanIf_Init(const CanIf_ConfigType *config);

/** Polls the driver and dispatches up to CANIF_MAX_RX_PER_CYCLE frames. */
void CanIf_MainFunctionRx(void);

/** Sends a frame through the driver. Returns 0 on success. */
int CanIf_Transmit(uint32_t canId, const uint8_t *data, uint8_t dlc);

void CanIf_GetStatistics(CanIf_StatisticsType *stats);

#ifdef __cplusplus
}
#endif

#endif /* BMS_BSW_CANIF_H */
