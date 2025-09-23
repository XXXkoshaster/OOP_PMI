#include "gtest/gtest.h"
#include "bits_counter.h"

TEST(count_bits, SmallRange) {
    EXPECT_EQ(count_bits(2, 7), 11);
}

TEST(count_bits, SingleNumber) {
    EXPECT_EQ(count_bits(5, 5), 2);
}

TEST(count_bits, ZeroCase) {
    EXPECT_EQ(count_bits(0, 0), 0);
}

TEST(count_bits, LargerRange) {
    EXPECT_EQ(count_bits(1, 4), 5); 
}