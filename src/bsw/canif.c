/**
 * @file canif.c
 * @brief CAN interface routing.
 */
#include "bms/bsw/canif.h"

#include <stddef.h>
#include <string.h>

static const CanIf_ConfigType *canIf_config = NULL;
static CanIf_StatisticsType canIf_stats;

static const CanIf_RxPduConfigType *canIf_findRxPdu(uint32_t canId)
{
    const CanIf_RxPduConfigType *found = NULL;
    if (canIf_config != NULL) {
        for (uint16_t i = 0U; (i < canIf_config->numRxPdus) && (found == NULL); i++) {
            if (canIf_config->rxPdus[i].canId == canId) {
                found = &canIf_config->rxPdus[i];
            }
        }
    }
    return found;
}

void CanIf_Init(const CanIf_ConfigType *config)
{
    canIf_config = config;
    (void)memset(&canIf_stats, 0, sizeof(canIf_stats));
}

void CanIf_MainFunctionRx(void)
{
    Can_FrameType frame;

    for (uint32_t n = 0U; n < CANIF_MAX_RX_PER_CYCLE; n++) {
        if (CanDrv_Read(&frame) != CAN_OK) {
            break;
        }
        const CanIf_RxPduConfigType *pdu = canIf_findRxPdu(frame.id);
        if (pdu == NULL) {
            canIf_stats.unknownId++;
        } else if (frame.dlc < pdu->minDlc) {
            canIf_stats.badDlc++;
        } else {
            canIf_stats.routed++;
            if (pdu->rxIndication != NULL) {
                pdu->rxIndication(pdu->pduId, frame.data, frame.dlc);
            }
        }
    }
}

int CanIf_Transmit(uint32_t canId, const uint8_t *data, uint8_t dlc)
{
    Can_FrameType frame;
    int result = -1;

    if ((data != NULL) && (dlc <= CANDRV_MAX_DLC)) {
        (void)memset(&frame, 0, sizeof(frame));
        frame.id = canId;
        frame.dlc = dlc;
        (void)memcpy(frame.data, data, dlc);
        if (CanDrv_Write(&frame) == CAN_OK) {
            result = 0;
        }
    }
    return result;
}

void CanIf_GetStatistics(CanIf_StatisticsType *stats)
{
    if (stats != NULL) {
        *stats = canIf_stats;
    }
}
