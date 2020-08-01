/*
 * File:   bit_set_unittest.cc
 * Author: zhujun
 */
#include <stddef.h>
#include "bit_set.h"
#include "log.h"
#include "gtest/gtest.h"

using namespace dsblock;

TEST(BitSet, Common) {
    BitSet bitset(1000);
    EXPECT_TRUE(bitset.Set(10));
    EXPECT_TRUE(bitset.Get(10));

    EXPECT_TRUE(bitset.Set(500));
    EXPECT_TRUE(bitset.Get(500));

    EXPECT_FALSE(bitset.Get(700));
}

TEST(BitSet, Zero) {
    BitSet bitset(1000);
    EXPECT_TRUE(bitset.Set(0));
    EXPECT_TRUE(bitset.Get(0));

    BitSet bitset2(0);
    EXPECT_EQ(bitset2.BitSize(), 8);
    EXPECT_EQ(bitset2.ByteSize(), 1);
    EXPECT_TRUE(bitset2.Set(0));
    EXPECT_TRUE(bitset2.Get(0));
}

TEST(BitSet, OutOfRange) {
    BitSet bitset(1000);

    EXPECT_FALSE(bitset.Set(1000));
    EXPECT_FALSE(bitset.Get(1000));

    EXPECT_FALSE(bitset.Set(10000));
    EXPECT_FALSE(bitset.Get(10000));
}

TEST(BitSet, Negative) {
    BitSet bitset(1000);

    EXPECT_FALSE(bitset.Set(-1));
    EXPECT_FALSE(bitset.Get(-1));

    EXPECT_FALSE(bitset.Set(-10000));
    EXPECT_FALSE(bitset.Get(-10000));
}
