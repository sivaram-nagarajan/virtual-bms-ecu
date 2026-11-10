#include "bms/platform/os.h"

#include <cstdint>

#include <gtest/gtest.h>

namespace {
int g_fast = 0;
int g_slow = 0;
void Fast() { ++g_fast; }
void Slow() { ++g_slow; }
const Os_ConfigType kConfig{Fast, Slow};

class OsTest : public ::testing::Test {
protected:
    void SetUp() override {
        g_fast = 0;
        g_slow = 0;
        Os_Init(&kConfig, 1000U);
    }
};
}  // namespace

// @verifies REQ-OS-001
TEST_F(OsTest, Runs10msEveryTickAnd100msEveryTenthTick) {
    for (std::uint32_t i = 0U; i < 25U; ++i) {
        Os_Tick(1000U + (i * OS_TICK_MS));
    }
    EXPECT_EQ(g_fast, 25);
    EXPECT_EQ(g_slow, 3);  // ticks 0, 10, 20
    EXPECT_EQ(Os_GetTickCount(), 25U);
    EXPECT_EQ(Os_GetOverrunCount(), 0U);
}

// @verifies REQ-OS-002
TEST_F(OsTest, CountsOverrunWhenTickIsLateByAPeriodAndResynchronises) {
    Os_Tick(1000U);
    Os_Tick(1010U + 9U);   // 9 ms late: jitter, not an overrun
    EXPECT_EQ(Os_GetOverrunCount(), 0U);
    Os_Tick(1020U + 15U);  // 15 ms late: overrun
    EXPECT_EQ(Os_GetOverrunCount(), 1U);
    Os_Tick(1035U + 10U);  // on time relative to the new release point
    EXPECT_EQ(Os_GetOverrunCount(), 1U);
}

// @verifies REQ-OS-002
TEST_F(OsTest, HandlesMillisecondCounterWrapAround) {
    Os_Init(&kConfig, UINT32_MAX - 5U);
    Os_Tick(UINT32_MAX - 5U);
    Os_Tick(4U);  // 10 ms later, after wrap
    EXPECT_EQ(Os_GetOverrunCount(), 0U);
    EXPECT_EQ(g_fast, 2);
}
