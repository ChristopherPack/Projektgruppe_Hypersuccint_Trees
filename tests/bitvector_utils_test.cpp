#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#include "pht/bitvector_utils.h"

TEST(BitvectorUtilsTest, NumberToBitvectorTest) {
    std::vector<bool> b = pht::Bitvector_Utils::numberToBitvector(2);
    EXPECT_THAT(b, ::testing::ElementsAre(true,false));
    b = pht::Bitvector_Utils::numberToBitvector(5);
    EXPECT_THAT(b, ::testing::ElementsAre(true,false,true));
    b = pht::Bitvector_Utils::numberToBitvector(3);
    EXPECT_THAT(b, ::testing::ElementsAre(true,true));
    b = pht::Bitvector_Utils::numberToBitvector(0);
    EXPECT_THAT(b, ::testing::ElementsAre(false));
}

TEST(BitvectorUtilsTest, BitvectorToNumberTest) {
    uint32_t b = pht::Bitvector_Utils::bitvectorToNumber({true,false});
    EXPECT_EQ(b, 2);
    b = pht::Bitvector_Utils::bitvectorToNumber({true,false,true});
    EXPECT_EQ(b, 5);
    b = pht::Bitvector_Utils::bitvectorToNumber({true,true});
    EXPECT_EQ(b, 3);
    b = pht::Bitvector_Utils::bitvectorToNumber({true});
    EXPECT_EQ(b, 1);
    b = pht::Bitvector_Utils::bitvectorToNumber({true,false,false});
    EXPECT_EQ(b, 4);
    b = pht::Bitvector_Utils::bitvectorToNumber({true,true,false});
    EXPECT_EQ(b, 6);
    b = pht::Bitvector_Utils::bitvectorToNumber({true,false,false,true,true,false,true,true,true,true,false,true,true,true});
    EXPECT_EQ(b, 9975);
}

TEST(BitvectorUtilsTest, NumberToBitvectorToNumberTest) {
    uint32_t b = 5;
    std::vector<bool> bit = pht::Bitvector_Utils::numberToBitvector(b);
    uint32_t c = pht::Bitvector_Utils::bitvectorToNumber(bit);
    EXPECT_EQ(c, b);
    b = 1;
    bit = pht::Bitvector_Utils::numberToBitvector(b);
    c = pht::Bitvector_Utils::bitvectorToNumber(bit);
    EXPECT_EQ(c, b);
    b = 230;
    bit = pht::Bitvector_Utils::numberToBitvector(b);
    c = pht::Bitvector_Utils::bitvectorToNumber(bit);
    EXPECT_EQ(c, b);
    b = 0;
    bit = pht::Bitvector_Utils::numberToBitvector(b);
    c = pht::Bitvector_Utils::bitvectorToNumber(bit);
    EXPECT_EQ(c, b);
    b = 5120448;
    bit = pht::Bitvector_Utils::numberToBitvector(b);
    c = pht::Bitvector_Utils::bitvectorToNumber(bit);
    EXPECT_EQ(c, b);
}

TEST(BitvectorUtilsTest, DecodeEliasGammaTest) {
    std::vector<bool> bitvector = {0,0,1,1,0};
    std::vector<bool>::iterator iterator = bitvector.begin();
    uint32_t res = pht::Bitvector_Utils::decodeEliasGamma(iterator);
    EXPECT_EQ(res, 6);
    bitvector = {0,0,0,0,0,0,0,0,0,0,0,0,0,true,false,false,true,true,false,true,true,true,true,false,true,true,true};
    iterator = bitvector.begin();
    res = pht::Bitvector_Utils::decodeEliasGamma(iterator);
    EXPECT_EQ(res, 9975);
    bitvector = {1};
    iterator = bitvector.begin();
    res = pht::Bitvector_Utils::decodeEliasGamma(iterator);
    EXPECT_EQ(res, 1);
    bitvector = {0,1,0};
    iterator = bitvector.begin();
    res = pht::Bitvector_Utils::decodeEliasGamma(iterator);
    EXPECT_EQ(res, 2);
    bitvector = {0,0,1,0,1};
    iterator = bitvector.begin();
    res = pht::Bitvector_Utils::decodeEliasGamma(iterator);
    EXPECT_EQ(res, 5);
}

TEST(BitvectorUtilsTest, FindEliasGammaTest) {
    std::vector<bool> bitvector = {0,1,1,1,1,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,1,0,0,1,0,1,0,0,0,1,0,1,1,0,0,0,0,1,1,0,1,1,0,1,1,0,0,1,0,1,0,0,0,1,0,1,1,0,0,0,0,1,1,0,1,1,1,0,0,1,0,1,1,0,0,0};
    std::vector<bool> res = pht::Bitvector_Utils::findEliasGammaIndex(bitvector, 0,2);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,0,1,0,0));
    res = pht::Bitvector_Utils::findEliasGammaIndex(bitvector, 1,2);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,0,0));
    res = pht::Bitvector_Utils::findEliasGammaIndex(bitvector, 2,2);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1,0,1,0,0,0));
    res = pht::Bitvector_Utils::findEliasGammaIndex(bitvector, 3,2);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0));
    res = pht::Bitvector_Utils::findEliasGammaIndex(bitvector, 4,2);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1,0,0,1,0,1,0,0));

    std::vector<bool> bitvector2 = {0,0,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,0,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0};
    res = pht::Bitvector_Utils::findEliasGammaIndex(bitvector2, 0,1);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,1,1,1));
    res = pht::Bitvector_Utils::findEliasGammaIndex(bitvector2, 1,1);
    EXPECT_THAT(res, ::testing::ElementsAre(1));
    res = pht::Bitvector_Utils::findEliasGammaIndex(bitvector2, 2,1);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1));
}

TEST(BitvectorUtilsTest, findBitvectorBitIndexTest) {
    std::vector<bool> fids = {0,0,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,0,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0};
    std::vector<bool> bitvector = {1,0,1,0,1,0,0,1,1,1,1,1};
    std::vector<bool> res = pht::Bitvector_Utils::findBitvectorBitIndex(bitvector,fids,0);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,1,0));
    res = pht::Bitvector_Utils::findBitvectorBitIndex(bitvector,fids,1);
    EXPECT_THAT(res, ::testing::ElementsAre(1));
    res = pht::Bitvector_Utils::findBitvectorBitIndex(bitvector,fids,2);
    EXPECT_THAT(res, ::testing::ElementsAre(0,0));

}

TEST(BitvectorUtilsTest, findBitvectorLengthTest) {
    std::vector<bool> fids = {0,0,1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,0,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,0,0,1,0,0,1,1,0,0};
    uint32_t res = pht::Bitvector_Utils::findBitvectorLength(fids, 0);
    EXPECT_EQ(res, 4);
}

TEST(BitvectorUtilsTest, getMicroDummyTest) {
    std::vector<bool> bitvector = {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::vector<bool> res = pht::Bitvector_Utils::findStaticSizeIndex(bitvector,0,4);
    EXPECT_THAT(res, ::testing::ElementsAre(0,0,0,0));
    res = pht::Bitvector_Utils::findStaticSizeIndex(bitvector,2,4);
    EXPECT_THAT(res, ::testing::ElementsAre(0,0,1,1));
}

TEST(BitvectorUtilsTest, createEliasGammaTest) {
    std::vector<bool> bitvector;
    uint32_t size = 6;
    pht::Bitvector_Utils::createEliasGamma(bitvector, size);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,1,1,0));
    bitvector.clear();
    size = 1;
    pht::Bitvector_Utils::createEliasGamma(bitvector, size);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(1));
    bitvector.clear();
    size = 5;
    pht::Bitvector_Utils::createEliasGamma(bitvector, size);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,1,0,1));
    bitvector.clear();
    size = 2;
    pht::Bitvector_Utils::createEliasGamma(bitvector, size);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,1,0));
    bitvector.clear();
    size = 9975;
    pht::Bitvector_Utils::createEliasGamma(bitvector, size);
    EXPECT_THAT(bitvector, ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0,0,true,false,false,true,true,false,true,true,true,true,false,true,true,true));
}
