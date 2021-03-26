#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#include "pht/bitvector_utils.h"
#include "pht/list_utils.h"

TEST(BitvectorUtilsTest, EncodeEliasGammaTest) {
    std::vector<bool> result;
    EXPECT_DEATH(pht::BitvectorUtils::encodeEliasGamma(result, 0), "Elias gamma undefined for 0");
    result.clear();
    pht::BitvectorUtils::encodeEliasGamma(result, 1);
    EXPECT_THAT(result, ::testing::ElementsAre(1));
    result.clear();
    pht::BitvectorUtils::encodeEliasGamma(result, 5);
    EXPECT_THAT(result, ::testing::ElementsAre(0,0,1,0,1));
    result.clear();
    pht::BitvectorUtils::encodeEliasGamma(result, 99);
    EXPECT_THAT(result, ::testing::ElementsAre(0,0,0,0,0,0,1,1,0,0,0,1,1));
    result.clear();
    pht::BitvectorUtils::encodeEliasGamma(result, 364);
    EXPECT_THAT(result, ::testing::ElementsAre(0,0,0,0,0,0,0,0,1,0,1,1,0,1,1,0,0));
    result.clear();
    pht::BitvectorUtils::encodeEliasGamma(result, 7395);
    EXPECT_THAT(result, ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,1,1));
    result.clear();
    pht::BitvectorUtils::encodeEliasGamma(result, 56789);
    EXPECT_THAT(result, ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1));
}

TEST(BitvectorUtilsTest, DecodeEliasGammaTest) {
    std::vector<bool> result;
    std::vector<bool>::const_iterator iterator;
    pht::ListUtils::combine(result, {1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeEliasGamma(iterator), 1);
    result.clear();
    pht::ListUtils::combine(result, {0,0,1,0,1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeEliasGamma(iterator), 5);
    result.clear();
    pht::ListUtils::combine(result, {0,0,0,0,0,0,1,1,0,0,0,1,1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeEliasGamma(iterator), 99);
    result.clear();
    pht::ListUtils::combine(result, {0,0,0,0,0,0,0,0,1,0,1,1,0,1,1,0,0});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeEliasGamma(iterator), 364);
    result.clear();
    pht::ListUtils::combine(result, {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,1,1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeEliasGamma(iterator), 7395);
    result.clear();
    pht::ListUtils::combine(result, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeEliasGamma(iterator), 56789);
}

TEST(BitvectorUtilsTest, EliasGammaReversiblityTest) {
    std::vector<bool> result;
    std::vector<bool>::const_iterator iterator;
    for(uint32_t i : {1, 5, 99, 364, 7395, 56789}) {
        result.clear();
        pht::BitvectorUtils::encodeEliasGamma(result, i);
        iterator = result.cbegin();
        EXPECT_EQ(pht::BitvectorUtils::decodeEliasGamma(iterator), i);
    }
}

TEST(BitvectorUtilsTest, EncodeBinaryTest) {
    std::vector<bool> result;
    pht::BitvectorUtils::encodeBinary(result, 0);
    EXPECT_THAT(result, ::testing::ElementsAre(0));
    result.clear();
    pht::BitvectorUtils::encodeBinary(result, 1);
    EXPECT_THAT(result, ::testing::ElementsAre(1));
    result.clear();
    pht::BitvectorUtils::encodeBinary(result, 5);
    EXPECT_THAT(result, ::testing::ElementsAre(1,0,1));
    result.clear();
    pht::BitvectorUtils::encodeBinary(result, 99);
    EXPECT_THAT(result, ::testing::ElementsAre(1,1,0,0,0,1,1));
    result.clear();
    pht::BitvectorUtils::encodeBinary(result, 364);
    EXPECT_THAT(result, ::testing::ElementsAre(1,0,1,1,0,1,1,0,0));
    result.clear();
    pht::BitvectorUtils::encodeBinary(result, 7395);
    EXPECT_THAT(result, ::testing::ElementsAre(1,1,1,0,0,1,1,1,0,0,0,1,1));
    result.clear();
    pht::BitvectorUtils::encodeBinary(result, 56789);
    EXPECT_THAT(result, ::testing::ElementsAre(1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1));
}

TEST(BitvectorUtilsTest, DecodeBinaryTest) {
    std::vector<bool> result;
    std::vector<bool>::const_iterator iterator;
    pht::ListUtils::combine(result, {0});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeBinary(iterator, result.size()), 0);
    result.clear();
    pht::ListUtils::combine(result, {1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeBinary(iterator, result.size()), 1);
    result.clear();
    pht::ListUtils::combine(result, {1,0,1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeBinary(iterator, result.size()), 5);
    result.clear();
    pht::ListUtils::combine(result, {1,1,0,0,0,1,1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeBinary(iterator, result.size()), 99);
    result.clear();
    pht::ListUtils::combine(result, {1,0,1,1,0,1,1,0,0});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeBinary(iterator, result.size()), 364);
    result.clear();
    pht::ListUtils::combine(result, {1,1,1,0,0,1,1,1,0,0,0,1,1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeBinary(iterator, result.size()), 7395);
    result.clear();
    pht::ListUtils::combine(result, {1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1});
    iterator = result.cbegin();
    EXPECT_EQ(pht::BitvectorUtils::decodeBinary(iterator, result.size()), 56789);
}

TEST(BitvectorUtilsTest, BinaryReversiblityTest) {
    std::vector<bool> result;
    std::vector<bool>::const_iterator iterator;
    for(uint32_t i : {1, 5, 99, 364, 7395, 56789}) {
        result.clear();
        pht::BitvectorUtils::encodeBinary(result, i);
        iterator = result.cbegin();
        EXPECT_EQ(pht::BitvectorUtils::decodeBinary(iterator, result.size()), i);
    }
}

TEST(BitvectorUtilsTest, AccessEliasGammaBitvectorTest) {
    std::vector<bool> result;
    std::vector<bool>::const_iterator iterator = result.cbegin();
    EXPECT_DEATH(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 0, 0), "Muliplier cannot be 0");
    pht::ListUtils::combine(result, {0,0,1,0,0,1,0,1,0,  0,0,1,0,1,0,1,0,0,1,  0,0,1,0,0,1,1,1,1,  0,0,1,1,0,0,0,0,0,0,0});
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 0), ::testing::ElementsAre(1,0,1,0));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 1), ::testing::ElementsAre(0,1,0,0,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 2), ::testing::ElementsAre(1,1,1,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 3), ::testing::ElementsAre(0,0,0,0,0,0));
    result.clear();
    pht::ListUtils::combine(result, {0,0,1,0,0,1,0,1,0,1,0,1,0,  0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,  0,0,1,0,0,1,1,1,1,1,1,1,1,  0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0});
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 0, 2), ::testing::ElementsAre(1,0,1,0,1,0,1,0));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 1, 2), ::testing::ElementsAre(0,1,0,0,1,0,1,0,0,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 2, 2), ::testing::ElementsAre(1,1,1,1,1,1,1,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 3, 2), ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0));
    result.clear();
    pht::ListUtils::combine(result, {0,0,1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,  0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,  0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,  0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 0, 3), ::testing::ElementsAre(1,0,1,0,1,0,1,0,1,0,1,0));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 1, 3), ::testing::ElementsAre(0,1,0,0,1,0,1,0,0,1,0,1,0,0,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 2, 3), ::testing::ElementsAre(1,1,1,1,1,1,1,1,1,1,1,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::accessEliasGammaBitvector(iterator, 3, 3), ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));
}

TEST(BitvectorUtilsTest, FindInBitvectorTest) {
    std::vector<bool> result;
    std::vector<bool>::const_iterator iterator = result.cbegin();
    EXPECT_DEATH(pht::BitvectorUtils::findInBitvector(iterator, 0, 0), "Size cannot be 0");
    pht::ListUtils::combine(result, {0,1,1,0,0,0,0,1,0,1,1});
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 0,  1), ::testing::ElementsAre(0));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 1,  1), ::testing::ElementsAre(1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 5,  1), ::testing::ElementsAre(0));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 10, 1), ::testing::ElementsAre(1));
    result.clear();
    pht::ListUtils::combine(result, {1,0,1,0,1,0,1,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,1,1,1,1,1,1});
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 0,  5), ::testing::ElementsAre(1,0,1,0,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 1,  5), ::testing::ElementsAre(0,1,0,0,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 5,  5), ::testing::ElementsAre(1,1,1,1,1));
    result.clear();
    pht::ListUtils::combine(result, {1,0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,1,0,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1});
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 0,  10), ::testing::ElementsAre(1,0,1,0,1,1,0,1,0,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 1,  10), ::testing::ElementsAre(0,1,0,0,1,0,1,0,0,1));
    iterator = result.cbegin();
    EXPECT_THAT(pht::BitvectorUtils::findInBitvector(iterator, 3,  10), ::testing::ElementsAre(1,1,1,1,1,1,1,1,1,1));
    
}

TEST(BitvectorUtilsTest, CountBitOccurrencesTest) {
    std::vector<bool> result;
    std::vector<bool>::const_iterator start;
    std::vector<bool>::const_iterator end;
    pht::ListUtils::combine(result, {0});
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end), 0);
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end, true), 1);
    result.clear();
    pht::ListUtils::combine(result, {1});
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end), 1);
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end, true), 0);
    result.clear();
    pht::ListUtils::combine(result, {1,0,1});
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end), 2);
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end, true), 1);
    result.clear();
    pht::ListUtils::combine(result, {1,1,0,0,0,1,1});
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end), 4);
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end, true), 3);
    result.clear();
    pht::ListUtils::combine(result, {1,0,1,1,0,1,1,0,0});
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end), 5);
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end, true), 4);
    result.clear();
    pht::ListUtils::combine(result, {1,1,1,0,0,1,1,1,0,0,0,1,1});
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end), 8);
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end, true), 5);
    result.clear();
    pht::ListUtils::combine(result, {1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1});
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end), 11);
    start = result.cbegin();
    end = result.cend();
    EXPECT_EQ(pht::BitvectorUtils::countBitOccurrences(start, end, true), 5);
}