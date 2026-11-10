/**
 * @file com.c
 * @brief Com skeleton: counts received PDUs. Signal handling follows in PR 2.
 */
#include "bms/bsw/com.h"

#include <string.h>

#define COM_MAX_RX_PDUS (16U)

static uint32_t com_rxCount[COM_MAX_RX_PDUS];

void Com_Init(void)
{
    (void)memset(com_rxCount, 0, sizeof(com_rxCount));
}

void Com_RxIndication(uint16_t pduId, const uint8_t *data, uint8_t dlc)
{
    (void)data;
    (void)dlc;
    if (pduId < COM_MAX_RX_PDUS) {
        com_rxCount[pduId]++;
    }
}

void Com_MainFunctionTx(void)
{
    /* BMS_Status and BMS_Limits transmission: PR 2. */
}

uint32_t Com_GetRxCount(uint16_t pduId)
{
    return (pduId < COM_MAX_RX_PDUS) ? com_rxCount[pduId] : 0U;
}
