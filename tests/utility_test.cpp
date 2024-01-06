#include <gtest/gtest.h>
#include "utility.h"

TEST(UtilityTest, AlignsProperly) {

    EXPECT_EQ(util::align(3, 8), 8);
    EXPECT_EQ(util::align(9, 8), 16);
    EXPECT_EQ(util::align(16, 8), 16);
    EXPECT_EQ(util::align(17, 8), 24);
    EXPECT_EQ(util::align(0, 8), 0);
}

TEST(UtilityTest, NumberCreation) {
    EXPECT_EQ(util::create_unsigned_num(0), 0x00000000u);
    EXPECT_EQ(util::create_unsigned_num(1), 0x01010101u);
    EXPECT_EQ(util::create_unsigned_num(2), 0x02020202u);
    EXPECT_EQ(util::create_unsigned_num(3), 0x03030303u);
    EXPECT_EQ(util::create_unsigned_num(0x33), 0x33333333u);
    EXPECT_EQ(util::create_unsigned_num(33), 0x21212121u);

}