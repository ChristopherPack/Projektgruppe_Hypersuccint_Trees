#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#include "pht/bitvector.h"

TEST(BitvectorTest, ConstructorTest) {
    pht::Bitvector bitvector;
    EXPECT_EQ(bitvector.size, 8);
    pht::Bitvector bitvector2(88);
    EXPECT_EQ(bitvector2.size, 88);
}

TEST(BitvectorTest, ResizeTest) {
    pht::Bitvector bitvector;
    EXPECT_EQ(bitvector.size, 8);
    bitvector.resize(16);
    EXPECT_EQ(bitvector.size, 16);
}

TEST(BitvectorTest, SetTest) {
    pht::Bitvector bitvector;
    EXPECT_EQ(bitvector.data[0], 0x0);
    bitvector.set(2);
    EXPECT_EQ(bitvector.data[0], 0x4);
}

TEST(BitvectorTest, ToggleTest) {
    pht::Bitvector bitvector;
    EXPECT_EQ(bitvector.data[0], 0x0);
    bitvector.toggle(2);
    EXPECT_EQ(bitvector.data[0], 0x4);
    bitvector.toggle(2);
    EXPECT_EQ(bitvector.data[0], 0x0);
}

TEST(BitvectorTest, ResetTest) {
    pht::Bitvector bitvector;
    EXPECT_EQ(bitvector.data[0], 0x0);
    bitvector.set(2);
    EXPECT_EQ(bitvector.data[0], 0x4);
    bitvector.reset(2);
    EXPECT_EQ(bitvector.data[0], 0x0);
}

TEST(BitvectorTest, AtTest) {
    pht::Bitvector bitvector;
    EXPECT_FALSE(bitvector.at(2));
    bitvector.set(2);
    EXPECT_TRUE(bitvector.at(2));
}