#include "bms/bsw/crc8_j1850.h"

#include <array>
#include <cstdint>

#include <gtest/gtest.h>

namespace {
constexpr std::array<std::uint8_t, 9> kCheckInput{'1', '2', '3', '4', '5', '6', '7', '8', '9'};
}  // namespace

// @verifies REQ-CRC-001
TEST(Crc8J1850, MatchesStandardCheckValue) {
    EXPECT_EQ(Crc_CalculateCRC8(kCheckInput.data(), kCheckInput.size(), 0U, true), 0x4BU);
}

// @verifies REQ-CRC-001
TEST(Crc8J1850, MatchesAutosarReferenceVectors) {
    // Values from AUTOSAR SWS CRC Library, CRC8 (SAE J1850) examples.
    const std::array<std::uint8_t, 4> zeros{0x00U, 0x00U, 0x00U, 0x00U};
    const std::array<std::uint8_t, 3> pattern{0xF2U, 0x01U, 0x83U};
    EXPECT_EQ(Crc_CalculateCRC8(zeros.data(), zeros.size(), 0U, true), 0x59U);
    EXPECT_EQ(Crc_CalculateCRC8(pattern.data(), pattern.size(), 0U, true), 0x37U);
}

// @verifies REQ-CRC-002
TEST(Crc8J1850, NullPointerReturnsEmptyBlockCrc) {
    EXPECT_EQ(Crc_CalculateCRC8(nullptr, 5U, 0U, true), 0x00U);
    EXPECT_EQ(Crc_CalculateCRC8(nullptr, 0U, 0U, true), 0x00U);
}

// @verifies REQ-CRC-003
TEST(Crc8J1850, ChainedCallEqualsSingleCall) {
    const std::uint8_t first = Crc_CalculateCRC8(kCheckInput.data(), 4U, 0U, true);
    const std::uint8_t chained = Crc_CalculateCRC8(kCheckInput.data() + 4, 5U, first, false);
    EXPECT_EQ(chained, 0x4BU);
}
