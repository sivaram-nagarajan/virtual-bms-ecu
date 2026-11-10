/**
 * @file ecu_cfg.c
 * @brief CanIf routing table. Minimum DLCs match dbc/bms.dbc.
 */
#include "ecu_cfg.h"

#include "bms/bsw/cantp.h"
#include "bms/bsw/com.h"

static const CanIf_RxPduConfigType ecu_rxPdus[] = {
    {ECU_CANID_VCU_COMMAND, 8U, (uint16_t)ECU_PDU_VCU_COMMAND, Com_RxIndication},
    {ECU_CANID_CMC_CELLV_1, 8U, (uint16_t)ECU_PDU_CMC_CELLV_1, Com_RxIndication},
    {ECU_CANID_CMC_CELLV_2, 8U, (uint16_t)ECU_PDU_CMC_CELLV_2, Com_RxIndication},
    {ECU_CANID_CMC_CELLV_3, 8U, (uint16_t)ECU_PDU_CMC_CELLV_3, Com_RxIndication},
    {ECU_CANID_CMC_TEMPERATURES, 4U, (uint16_t)ECU_PDU_CMC_TEMPERATURES, Com_RxIndication},
    {ECU_CANID_PACK_SENSOR, 4U, (uint16_t)ECU_PDU_PACK_SENSOR, Com_RxIndication},
    {ECU_CANID_DIAG_REQUEST, 1U, (uint16_t)ECU_PDU_DIAG_REQUEST, CanTp_RxIndication},
};

const CanIf_ConfigType Ecu_CanIfConfig = {
    ecu_rxPdus,
    (uint16_t)(sizeof(ecu_rxPdus) / sizeof(ecu_rxPdus[0])),
};
