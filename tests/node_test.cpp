#include "gtest/gtest.h"

#include "pht/node.h"

TEST(NodeTest, GetSetValueTest) {
    std::shared_ptr<pht::Node<char>> node = std::make_shared<pht::Node<char>>('a');
    EXPECT_EQ(node->getValue(), 'a');
    node->setValue('b');
    EXPECT_EQ(node->getValue(), 'b');
}