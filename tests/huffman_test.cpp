//
// Created by User on 21.05.2021.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"
#include <map>

#include "pht/huffman.h"

TEST(HuffmanTest, generateTableTest) {
    std::map<char,uint32_t> occurrences;
    occurrences.insert({'A',11});
    occurrences.insert({'B',23});
    occurrences.insert({'C',20});
    occurrences.insert({'D',15});
    occurrences.insert({'E',10});
    occurrences.insert({'F',5});

    std::map<char,std::vector<bool>> res = pht::Huffman::generateTable(occurrences);

    EXPECT_THAT(res.at('A'), ::testing::ElementsAre(0,0,1));
    EXPECT_THAT(res.at('B'), ::testing::ElementsAre(0,1));
    EXPECT_THAT(res.at('C'), ::testing::ElementsAre(1,0));
    EXPECT_THAT(res.at('D'), ::testing::ElementsAre(0,0,0));
    EXPECT_THAT(res.at('E'), ::testing::ElementsAre(1,1,0));
    EXPECT_THAT(res.at('F'), ::testing::ElementsAre(1,1,1));
}