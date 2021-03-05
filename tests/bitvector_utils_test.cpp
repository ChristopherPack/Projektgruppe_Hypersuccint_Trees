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
