/**
 * @file cantp.c
 * @brief CanTp skeleton. ISO-TP implementation follows in PR 3.
 */
#include "bms/bsw/cantp.h"

void CanTp_Init(void) {}

void CanTp_RxIndication(uint16_t pduId, const uint8_t *data, uint8_t dlc)
{
    (void)pduId;
    (void)data;
    (void)dlc;
}

void CanTp_MainFunction(void) {}
