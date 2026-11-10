#include "bms/bsw/canif.h"

#include <array>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "can_drv_stub.h"

namespace {
struct Received {
    std::uint16_t pduId;
    std::uint8_t dlc;
    std::uint8_t firstByte;
};
std::vector<Received> g_upperA;
std::vector<Received> g_upperB;

void UpperA(std::uint16_t pduId, const std::uint8_t* data, std::uint8_t dlc) {
    g_upperA.push_back({pduId, dlc, data[0]});
}
void UpperB(std::uint16_t pduId, const std::uint8_t* data, std::uint8_t dlc) {
    g_upperB.push_back({pduId, dlc, data[0]});
}

const std::array<CanIf_RxPduConfigType, 2> kRxPdus{{
    {0x100U, 8U, 7U, UpperA},
    {0x7E0U, 1U, 3U, UpperB},
}};
const CanIf_ConfigType kConfig{kRxPdus.data(), static_cast<std::uint16_t>(kRxPdus.size())};

class CanIfTest : public ::testing::Test {
protected:
    void SetUp() override {
        g_upperA.clear();
        g_upperB.clear();
        CanDrvStub_Reset();
        CanIf_Init(&kConfig);
    }
};
}  // namespace

// @verifies REQ-CANIF-001
TEST_F(CanIfTest, RoutesFramesToConfiguredUpperLayerWithPduId) {
    const std::array<std::uint8_t, 8> a{0xAAU};
    const std::array<std::uint8_t, 2> b{0xBBU};
    CanDrvStub_InjectRx(0x100U, a.data(), 8U);
    CanDrvStub_InjectRx(0x7E0U, b.data(), 2U);

    CanIf_MainFunctionRx();

    ASSERT_EQ(g_upperA.size(), 1U);
    EXPECT_EQ(g_upperA[0].pduId, 7U);
    EXPECT_EQ(g_upperA[0].firstByte, 0xAAU);
    ASSERT_EQ(g_upperB.size(), 1U);
    EXPECT_EQ(g_upperB[0].pduId, 3U);
    EXPECT_EQ(g_upperB[0].dlc, 2U);
}

// @verifies REQ-CANIF-002
TEST_F(CanIfTest, DiscardsAndCountsUnknownIdsAndShortFrames) {
    const std::array<std::uint8_t, 8> d{};
    CanDrvStub_InjectRx(0x555U, d.data(), 8U);  // unknown
    CanDrvStub_InjectRx(0x100U, d.data(), 7U);  // too short (needs 8)

    CanIf_MainFunctionRx();

    EXPECT_TRUE(g_upperA.empty());
    CanIf_StatisticsType stats{};
    CanIf_GetStatistics(&stats);
    EXPECT_EQ(stats.unknownId, 1U);
    EXPECT_EQ(stats.badDlc, 1U);
    EXPECT_EQ(stats.routed, 0U);
}

// @verifies REQ-CANIF-001
TEST_F(CanIfTest, BoundsFramesProcessedPerCycle) {
    const std::array<std::uint8_t, 8> d{};
    for (std::uint32_t i = 0U; i < CANIF_MAX_RX_PER_CYCLE + 5U; ++i) {
        CanDrvStub_InjectRx(0x100U, d.data(), 8U);
    }
    CanIf_MainFunctionRx();
    EXPECT_EQ(g_upperA.size(), CANIF_MAX_RX_PER_CYCLE);
    CanIf_MainFunctionRx();
    EXPECT_EQ(g_upperA.size(), CANIF_MAX_RX_PER_CYCLE + 5U);
}

TEST_F(CanIfTest, TransmitPassesFrameToDriver) {
    const std::array<std::uint8_t, 3> d{1U, 2U, 3U};
    EXPECT_EQ(CanIf_Transmit(0x300U, d.data(), 3U), 0);
    ASSERT_EQ(CanDrvStub_TxCount(), 1U);
    EXPECT_EQ(CanDrvStub_TxFrame(0U)->id, 0x300U);
    EXPECT_EQ(CanDrvStub_TxFrame(0U)->data[2], 3U);
    EXPECT_EQ(CanIf_Transmit(0x300U, d.data(), 9U), -1);
}
