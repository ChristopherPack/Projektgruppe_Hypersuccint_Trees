#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#include "pht/list_utils.h"

TEST(ListUtilsTest, AddAllTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = {5,6,7,8,9};
    std::vector<int> c = {};
    pht::ListUtils::addAll(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4,5,6,7,8,9));
    EXPECT_THAT(b, ::testing::ElementsAre(5,6,7,8,9));
    pht::ListUtils::addAll(a, c);
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4,5,6,7,8,9));
    EXPECT_THAT(c, ::testing::ElementsAre());
    pht::ListUtils::addAll(c, a);
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4,5,6,7,8,9));
    EXPECT_THAT(c, ::testing::ElementsAre(0,1,2,3,4,5,6,7,8,9));
}

TEST(ListUtilsTest, ContainsTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = {5,6,7,8,9};
    std::vector<int> c = {};
    EXPECT_TRUE(pht::ListUtils::contains(a, 0));
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4));
    EXPECT_FALSE(pht::ListUtils::contains(a, 5));
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4));
    EXPECT_FALSE(pht::ListUtils::contains(b, 0));
    EXPECT_THAT(b, ::testing::ElementsAre(5,6,7,8,9));
    EXPECT_TRUE(pht::ListUtils::contains(b, 5));
    EXPECT_THAT(b, ::testing::ElementsAre(5,6,7,8,9));
    EXPECT_FALSE(pht::ListUtils::contains(c, 0));
    EXPECT_THAT(c, ::testing::ElementsAre());
}

TEST(ListUtilsTest, IndexOfTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = {5,6,7,8,9};
    std::vector<int> c = {};
    EXPECT_EQ(pht::ListUtils::indexOf(a, 0), 0);
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4));
    EXPECT_EQ(pht::ListUtils::indexOf(a, 3), 3);
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4));
    EXPECT_EQ(pht::ListUtils::indexOf(a, 5), -1);
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4));
    EXPECT_EQ(pht::ListUtils::indexOf(b, 0), -1);
    EXPECT_THAT(b, ::testing::ElementsAre(5,6,7,8,9));
    EXPECT_EQ(pht::ListUtils::indexOf(b, 5), 0);
    EXPECT_THAT(b, ::testing::ElementsAre(5,6,7,8,9));
    EXPECT_EQ(pht::ListUtils::indexOf(c, 0), -1);
    EXPECT_THAT(c, ::testing::ElementsAre());
}

TEST(ListUtilsTest, SortTest) {
    std::vector<int> a = {4,1,4,2,2};
    std::vector<int> b = {};
    pht::ListUtils::sort(a, [](int a, int b){ return a < b; });
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,2,4,4));
    pht::ListUtils::sort(b, [](int a, int b){ return a > b; });
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, FilterTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = {};
    pht::ListUtils::filter(a, [](int x){ return x >= 2; });
    EXPECT_THAT(a, ::testing::ElementsAre(2,3,4));
    pht::ListUtils::filter(b, [](int x){ return x >= 2; });
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, MapTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<char> b = {};
    std::vector<int> c = {};
    pht::ListUtils::map(a, b, [](int x){ return (char)(x+97); });
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4));
    EXPECT_THAT(b, ::testing::ElementsAre('a','b','c','d','e'));
    b.clear();
    pht::ListUtils::map(c, b, [](int x){ return (char)(x+97); });
    EXPECT_THAT(b, ::testing::ElementsAre());
    EXPECT_THAT(c, ::testing::ElementsAre());
}

TEST(ListUtilsTest, FoldTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = {};
    EXPECT_EQ(pht::ListUtils::fold(a, 1000, [](int acc, int x){ return acc+x; }), 1010);
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4));
    EXPECT_EQ(pht::ListUtils::fold(b, 1000, [](int acc, int x){ return acc+x; }), 1000);
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ForEachTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = {};
    std::vector<int> c = {};
    pht::ListUtils::forEach(a, [&c](int x)mutable{ c.push_back(x); });
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4));
    EXPECT_THAT(c, ::testing::ElementsAre(0,1,2,3,4));
    c.clear();
    pht::ListUtils::forEach(b, [&c](int x)mutable{ c.push_back(x); });
    EXPECT_THAT(c, ::testing::ElementsAre());
}

TEST(ListUtilsTest, EraseTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = {};
    pht::ListUtils::erase(a, 1);
    EXPECT_THAT(a, ::testing::ElementsAre(0,2,3,4));
    pht::ListUtils::erase(a, 7);
    EXPECT_THAT(a, ::testing::ElementsAre(0,2,3,4));
    pht::ListUtils::erase(b, 0);
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, EraseAllTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = {1,2,3};
    std::vector<int> c = {};
    std::vector<int> d = {5,6};
    pht::ListUtils::eraseAll(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre(0,4));
    EXPECT_THAT(b, ::testing::ElementsAre(1,2,3));
    pht::ListUtils::eraseAll(a, d);
    EXPECT_THAT(a, ::testing::ElementsAre(0,4));
    EXPECT_THAT(d, ::testing::ElementsAre(5,6));
    pht::ListUtils::eraseAll(c, d);
    EXPECT_THAT(c, ::testing::ElementsAre());
    EXPECT_THAT(d, ::testing::ElementsAre(5,6));
}

TEST(ListUtilsTest, ReverseTest) {
    std::vector<int> a = {0,1,2,3,4};
    std::vector<int> b = pht::ListUtils::reversed(a);
    EXPECT_THAT(b, ::testing::ElementsAre(4,3,2,1,0));
}

TEST(ListUtilsTest, DistinctTest) {
    std::vector<int> a = {0,0,2,3,3};
    std::vector<int> b;
    pht::ListUtils::distinct(a,b);
    EXPECT_THAT(b, ::testing::ElementsAre(0,2,3));
}