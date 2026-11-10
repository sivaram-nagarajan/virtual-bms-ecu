// Integration of the real ECU configuration: CanIf table -> Com / CanTp,
// driven through the task bodies exactly as the scheduler calls them.
#include <array>
#include <cstdint>

#include <gtest/gtest.h>

#include "bms/bsw/canif.h"
#include "bms/bsw/com.h"
#include "bms_tasks.h"
#include "can_drv_stub.h"
#include "ecu_cfg.h"

namespace {
class EcuIntegration : public ::testing::Test {
protected:
    void SetUp() override {
        CanDrvStub_Reset();
        Bms_InitAll();
    }
};
}  // namespace

// @verifies REQ-CANIF-001
TEST_F(EcuIntegration, EveryConfiguredApplicationFrameReachesCom) {
    const std::array<std::uint8_t, 8> d{};
    CanDrvStub_InjectRx(ECU_CANID_VCU_COMMAND, d.data(), 8U);
    CanDrvStub_InjectRx(ECU_CANID_CMC_CELLV_1, d.data(), 8U);
    CanDrvStub_InjectRx(ECU_CANID_CMC_CELLV_2, d.data(), 8U);
    CanDrvStub_InjectRx(ECU_CANID_CMC_CELLV_3, d.data(), 8U);
    CanDrvStub_InjectRx(ECU_CANID_CMC_TEMPERATURES, d.data(), 4U);
    CanDrvStub_InjectRx(ECU_CANID_PACK_SENSOR, d.data(), 4U);
    CanDrvStub_InjectRx(ECU_CANID_DIAG_REQUEST, d.data(), 8U);

    Bms_Task10ms();
    Bms_Task100ms();

    for (std::uint16_t pdu = ECU_PDU_VCU_COMMAND; pdu <= ECU_PDU_PACK_SENSOR; ++pdu) {
        EXPECT_EQ(Com_GetRxCount(pdu), 1U) << "PDU " << pdu;
    }
    // The diagnostic request goes to CanTp, not Com.
    EXPECT_EQ(Com_GetRxCount(ECU_PDU_DIAG_REQUEST), 0U);
    CanIf_StatisticsType stats{};
    CanIf_GetStatistics(&stats);
    EXPECT_EQ(stats.routed, 7U);
}

// @verifies REQ-CANIF-002
TEST_F(EcuIntegration, ShortCellVoltageFrameIsRejectedByConfiguredDlc) {
    const std::array<std::uint8_t, 8> d{};
    CanDrvStub_InjectRx(ECU_CANID_CMC_CELLV_1, d.data(), 6U);
    Bms_Task10ms();
    EXPECT_EQ(Com_GetRxCount(ECU_PDU_CMC_CELLV_1), 0U);
    EXPECT_EQ(Com_GetRxCount(999U), 0U);
}
