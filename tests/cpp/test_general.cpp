#include <gtest/gtest.h>

bool isCool = true;

TEST(MathTest, Addition) { EXPECT_EQ(1 + 1, 2); }

TEST(OtherTests, Coolness) { EXPECT_TRUE(isCool); }
