#include <gtest/gtest.h>
#include "utility.h"

TEST(UtilityTest, AlignsProperly) {

    EXPECT_EQ(util::align(3, 8), 8);
    EXPECT_EQ(util::align(9, 8), 16);
    EXPECT_EQ(util::align(16, 8), 16);
    EXPECT_EQ(util::align(17, 8), 24);
    EXPECT_EQ(util::align(0, 8), 0);
}