/**
 * @file ecu_cfg.h
 * @brief ECU-level configuration: CAN IDs and PDU handles (from dbc/bms.dbc).
 */
#ifndef BMS_ECU_CFG_H
#define BMS_ECU_CFG_H

#include "bms/bsw/canif.h"

#define ECU_CANID_VCU_COMMAND      (0x100U)
#define ECU_CANID_CMC_CELLV_1      (0x200U)
#define ECU_CANID_CMC_CELLV_2      (0x201U)
#define ECU_CANID_CMC_CELLV_3      (0x202U)
#define ECU_CANID_CMC_TEMPERATURES (0x210U)
#define ECU_CANID_PACK_SENSOR      (0x220U)
#define ECU_CANID_DIAG_REQUEST     (0x7E0U)
/* Tx IDs (BMS_Status 0x300, BMS_Limits 0x301, Diag_Response 0x7E8) are added
 * together with their senders in PR 2 and PR 3 (MISRA C:2012 Rule 2.5). */

/** Rx PDU handles used by Com and CanTp. */
enum {
    ECU_PDU_VCU_COMMAND = 0,
    ECU_PDU_CMC_CELLV_1,
    ECU_PDU_CMC_CELLV_2,
    ECU_PDU_CMC_CELLV_3,
    ECU_PDU_CMC_TEMPERATURES,
    ECU_PDU_PACK_SENSOR,
    ECU_PDU_DIAG_REQUEST,
    ECU_PDU_COUNT
};

extern const CanIf_ConfigType Ecu_CanIfConfig;

#endif /* BMS_ECU_CFG_H */
