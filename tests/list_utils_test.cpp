#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#include "pht/list_utils.h"

TEST(ListUtilsTest, CombineTest) {
    std::vector<uint32_t> a = {1,2,3,4,5,6,7};
    std::vector<uint32_t> b = {10,11,12,14,15,16,17};
    pht::ListUtils::combine(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7,10,11,12,14,15,16,17));
    EXPECT_THAT(b, ::testing::ElementsAre(10,11,12,14,15,16,17));
    a = {1,2,3,4,5,6,7};
    b = {1,2,3,4,5,6,7};
    pht::ListUtils::combine(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7,1,2,3,4,5,6,7));
    EXPECT_THAT(b, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {};
    b = {};
    pht::ListUtils::combine(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre());
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, Combine2Test) {
    std::vector<uint32_t> a = {1,2,3,4,5,6,7};
    pht::ListUtils::combine(a, {10,11,12,14,15,16,17});
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7,10,11,12,14,15,16,17));
    a = {1,2,3,4,5,6,7};
    pht::ListUtils::combine(a, {1,2,3,4,5,6,7});
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7,1,2,3,4,5,6,7));
    a = {};
    pht::ListUtils::combine(a, {});
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, CombinedTest) {
    std::vector<uint32_t> a = {1,2,3,4,5,6,7};
    std::vector<uint32_t> b = {10,11,12,14,15,16,17};
    EXPECT_THAT(pht::ListUtils::combined(a, b), ::testing::ElementsAre(1,2,3,4,5,6,7,10,11,12,14,15,16,17));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    EXPECT_THAT(b, ::testing::ElementsAre(10,11,12,14,15,16,17));
    a = {1,2,3,4,5,6,7};
    b = {1,2,3,4,5,6,7};
    EXPECT_THAT(pht::ListUtils::combined(a, b), ::testing::ElementsAre(1,2,3,4,5,6,7,1,2,3,4,5,6,7));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    EXPECT_THAT(b, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {};
    b = {};
    EXPECT_THAT(pht::ListUtils::combined(a, b), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, Combined2Test) {
    std::vector<uint32_t> a = {1,2,3,4,5,6,7};
    EXPECT_THAT(pht::ListUtils::combined(a, {10,11,12,14,15,16,17}), ::testing::ElementsAre(1,2,3,4,5,6,7,10,11,12,14,15,16,17));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {1,2,3,4,5,6,7};
    EXPECT_THAT(pht::ListUtils::combined(a, {1,2,3,4,5,6,7}), ::testing::ElementsAre(1,2,3,4,5,6,7,1,2,3,4,5,6,7));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {};
    EXPECT_THAT(pht::ListUtils::combined(a, {}), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());

    //TODO
    std::vector<bool> aa = {true};
    EXPECT_THAT(pht::ListUtils::combined(aa, {false}), ::testing::ElementsAre(true,false));
    EXPECT_THAT(aa, ::testing::ElementsAre(true));
}

TEST(ListUtilsTest, DistinctedTest) {
    std::vector<uint32_t> a = {1,2,2,4,5,6,6};
    EXPECT_THAT(pht::ListUtils::distincted(a), ::testing::ElementsAre(1,2,4,5,6));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,2,4,5,6,6));
    a = {1,2,3,3,3,3,3,3,0,0};
    EXPECT_THAT(pht::ListUtils::distincted(a), ::testing::ElementsAre(1,2,3,0));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,3,3,3,0,0));
    a = {};
    EXPECT_THAT(pht::ListUtils::distincted(a), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, DistinctTest) {
    std::vector<uint32_t> a = {1,2,2,4,5,6,6};
    pht::ListUtils::distinct(a);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,4,5,6));
    a = {1,2,3,3,3,3,3,3,0,0};
    pht::ListUtils::distinct(a);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,0));
    a = {};
    pht::ListUtils::distinct(a);
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ContainsAnyTest) {
    std::vector<uint32_t> a = {1,2,3,4,5,6,7};
    std::vector<uint32_t> b = {10,11,12,14,15,16,17};
    EXPECT_FALSE(pht::ListUtils::containsAny(a, b));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    EXPECT_THAT(b, ::testing::ElementsAre(10,11,12,14,15,16,17));
    a = {0,1,2,3,4,5,6};
    b = {1,2,3,4,5,6,7};
    EXPECT_TRUE(pht::ListUtils::containsAny(a, b));
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4,5,6));
    EXPECT_THAT(b, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {};
    b = {};
    EXPECT_TRUE(pht::ListUtils::containsAny(a, b));
    EXPECT_THAT(a, ::testing::ElementsAre());
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ContainsAny2Test) {
    std::vector<uint32_t> a = {1,2,3,4,5,6,7};
    EXPECT_FALSE(pht::ListUtils::containsAny(a, {10,11,12,14,15,16,17}));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {0,1,2,3,4,5,6};
    EXPECT_TRUE(pht::ListUtils::containsAny(a, {1,2,3,4,5,6,7}));
    EXPECT_THAT(a, ::testing::ElementsAre(0,1,2,3,4,5,6));
    a = {};
    EXPECT_TRUE(pht::ListUtils::containsAny(a, {}));
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ContainsAllTest) {
    std::vector<uint32_t> a = {1,2,3,4,5,6,7};
    std::vector<uint32_t> b = {1,11,12,14,15,16,17};
    EXPECT_FALSE(pht::ListUtils::containsAll(a, b));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    EXPECT_THAT(b, ::testing::ElementsAre(1,11,12,14,15,16,17));
    a = {1,2,3,4,5,6,7};
    b = {1,2,3,4,5,6,7};
    EXPECT_TRUE(pht::ListUtils::containsAll(a, b));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    EXPECT_THAT(b, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {};
    b = {};
    EXPECT_TRUE(pht::ListUtils::containsAll(a, b));
    EXPECT_THAT(a, ::testing::ElementsAre());
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ContainsAll2Test) {
    std::vector<uint32_t> a = {1,2,3,4,5,6,7};
    EXPECT_FALSE(pht::ListUtils::containsAll(a, {1,11,12,14,15,16,17}));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {1,2,3,4,5,6,7};
    EXPECT_TRUE(pht::ListUtils::containsAll(a, {1,2,3,4,5,6,7}));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5,6,7));
    a = {};
    EXPECT_TRUE(pht::ListUtils::containsAll(a, {}));
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, SortedTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    EXPECT_THAT(pht::ListUtils::sorted<uint32_t>(a, [](uint32_t a, uint32_t b){return a<b;}), ::testing::ElementsAre(1,2,3,4,5));
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    EXPECT_THAT(pht::ListUtils::sorted<uint32_t>(a, [](uint32_t a, uint32_t b){return a<b;}), ::testing::ElementsAre(0,0,1,2,2,3,3,3,3,3));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    EXPECT_THAT(pht::ListUtils::sorted<uint32_t>(a, [](uint32_t a, uint32_t b){return a>b;}), ::testing::ElementsAre(3,3,3,3,3,2,2,1,0,0));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    EXPECT_THAT(pht::ListUtils::sorted<uint32_t>(a, [](uint32_t a, uint32_t b){return a<b;}), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, SortTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    pht::ListUtils::sort<uint32_t>(a, [](uint32_t a, uint32_t b){return a<b;});
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,4,5));
    a = {1,2,3,3,3,2,3,3,0,0};
    pht::ListUtils::sort<uint32_t>(a, [](uint32_t a, uint32_t b){return a<b;});
    EXPECT_THAT(a, ::testing::ElementsAre(0,0,1,2,2,3,3,3,3,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    pht::ListUtils::sort<uint32_t>(a, [](uint32_t a, uint32_t b){return a>b;});
    EXPECT_THAT(a, ::testing::ElementsAre(3,3,3,3,3,2,2,1,0,0));
    a = {};
    pht::ListUtils::sort<uint32_t>(a, [](uint32_t a, uint32_t b){return a<b;});
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, FilterTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    pht::ListUtils::filter<uint32_t>(a, [](uint32_t x){return x==5;});
    EXPECT_THAT(a, ::testing::ElementsAre(5));
    a = {5,1,4,2,3};
    pht::ListUtils::filter<uint32_t>(a, [](uint32_t x){return x!=5;});
    EXPECT_THAT(a, ::testing::ElementsAre(1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    pht::ListUtils::filter<uint32_t>(a, [](uint32_t x){return x==5;});
    EXPECT_THAT(a, ::testing::ElementsAre());
    a = {};
    pht::ListUtils::filter<uint32_t>(a, [](uint32_t x){return x==5;});
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, FilteredTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    EXPECT_THAT(pht::ListUtils::filtered<uint32_t>(a, [](uint32_t x){return x==5;}), ::testing::ElementsAre(5));
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {5,1,4,2,3};
    EXPECT_THAT(pht::ListUtils::filtered<uint32_t>(a, [](uint32_t x){return x!=5;}), ::testing::ElementsAre(1,4,2,3));
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    EXPECT_THAT(pht::ListUtils::filtered<uint32_t>(a, [](uint32_t x){return x==5;}), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    EXPECT_THAT(pht::ListUtils::filtered<uint32_t>(a, [](uint32_t x){return x==5;}), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, MappedTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    std::vector<char> b = pht::ListUtils::mapped<uint32_t, char>(a, [](uint32_t x){return static_cast<char>(x+0x61);});
    EXPECT_THAT(b, ::testing::ElementsAre('f','b','e','c','d'));
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    b = pht::ListUtils::mapped<uint32_t, char>(a, [](uint32_t x){return static_cast<char>(x+0x61);});
    EXPECT_THAT(b, ::testing::ElementsAre('b','c','d','d','d','c','d','d','a','a'));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    b = pht::ListUtils::mapped<uint32_t, char>(a, [](uint32_t x){return static_cast<char>(x+0x61);});
    EXPECT_THAT(b, ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, MapTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    std::vector<char> b;
    pht::ListUtils::map<uint32_t, char>(a, b, [](uint32_t x){return static_cast<char>(x+0x61);});
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    EXPECT_THAT(b, ::testing::ElementsAre('f','b','e','c','d'));
    a = {1,2,3,3,3,2,3,3,0,0};
    b = {};
    pht::ListUtils::map<uint32_t, char>(a, b, [](uint32_t x){return static_cast<char>(x+0x61);});
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    EXPECT_THAT(b, ::testing::ElementsAre('b','c','d','d','d','c','d','d','a','a'));
    a = {};
    b = {};
    pht::ListUtils::map<uint32_t, char>(a, b, [](uint32_t x){return static_cast<char>(x+0x61);});
    EXPECT_THAT(a, ::testing::ElementsAre());
    EXPECT_THAT(b, ::testing::ElementsAre());
}

TEST(ListUtilsTest, FoldTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    double b = pht::ListUtils::fold<uint32_t, double>(a, 1.0,  [](double acc, uint32_t x){return acc+static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 16.0);
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    b = pht::ListUtils::fold<uint32_t, double>(a, 10.0, [](double acc, uint32_t x){return acc+static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 25.0);
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    b = pht::ListUtils::fold<uint32_t, double>(a, 1.0, [](double acc, uint32_t x){return acc*static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 120.0);
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    b = pht::ListUtils::fold<uint32_t, double>(a, 1.0, [](double acc, uint32_t x){return acc+static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 21.0);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    b = pht::ListUtils::fold<uint32_t, double>(a, 10.0, [](double acc, uint32_t x){return acc+static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 30.0);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    b = pht::ListUtils::fold<uint32_t, double>(a, 1.0, [](double acc, uint32_t x){return acc*static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 0.0);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    b = pht::ListUtils::fold<uint32_t, double>(a, 1.0, [](double acc, uint32_t x){return acc+static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 1.0);
    EXPECT_THAT(a, ::testing::ElementsAre());
    b = pht::ListUtils::fold<uint32_t, double>(a, 10.0, [](double acc, uint32_t x){return acc+static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 10.0);
    EXPECT_THAT(a, ::testing::ElementsAre());
    b = pht::ListUtils::fold<uint32_t, double>(a, 1.0, [](double acc, uint32_t x){return acc*static_cast<double>(x);});
    EXPECT_DOUBLE_EQ(b, 1.0);
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ForeachTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    uint32_t sum = 0;
    pht::ListUtils::foreach<uint32_t>(a, [&sum](uint32_t x){sum += x;});
    EXPECT_EQ(sum, 15);
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    sum = 0;
    pht::ListUtils::foreach<uint32_t>(a, [&sum](uint32_t x){sum += x;});
    EXPECT_EQ(sum, 20);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    sum = 0;
    pht::ListUtils::foreach<uint32_t>(a, [&sum](uint32_t x){sum += x;});
    EXPECT_EQ(sum, 0);
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, IndexOfTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    EXPECT_EQ(pht::ListUtils::indexOf(a, 0u), -1);
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    EXPECT_EQ(pht::ListUtils::indexOf(a, 0u), 8);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    EXPECT_EQ(pht::ListUtils::indexOf(a, 0u), -1);
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, EraseTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    std::vector<uint32_t> b = {5,4};
    pht::ListUtils::erase(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    b = {0,7};
    pht::ListUtils::erase(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    b = {};
    pht::ListUtils::erase(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    b = {};
    pht::ListUtils::erase(a, b);
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, Erase2Test) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    pht::ListUtils::erase(a, {5,4});
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    pht::ListUtils::erase(a, {0,7});
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    pht::ListUtils::erase(a, {});
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    pht::ListUtils::erase(a, {});
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ErasedTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    EXPECT_THAT(pht::ListUtils::erased(a, {5,4}), ::testing::ElementsAre(1,2,3));
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    EXPECT_THAT(pht::ListUtils::erased(a, {0,7}), ::testing::ElementsAre(1,2,3,3,3,2,3,3));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    EXPECT_THAT(pht::ListUtils::erased(a, {}), ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    EXPECT_THAT(pht::ListUtils::erased(a, {}), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());

}

TEST(ListUtilsTest, Erased2Test) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    std::vector<uint32_t> b = {5,4};
    EXPECT_THAT(pht::ListUtils::erased(a, b), ::testing::ElementsAre(1,2,3));
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    b = {0,7};
    EXPECT_THAT(pht::ListUtils::erased(a, b), ::testing::ElementsAre(1,2,3,3,3,2,3,3));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    b = {};
    EXPECT_THAT(pht::ListUtils::erased(a, b), ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    b = {};
    EXPECT_THAT(pht::ListUtils::erased(a, b), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ReverseTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    pht::ListUtils::reverse(a);
    EXPECT_THAT(a, ::testing::ElementsAre(3,2,4,1,5));
    a = {1,2,3,3,3,2,3,3,0,0};
    pht::ListUtils::reverse(a);
    EXPECT_THAT(a, ::testing::ElementsAre(0,0,3,3,2,3,3,3,2,1));
    a = {};
    pht::ListUtils::reverse(a);
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ReversedTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    EXPECT_THAT(pht::ListUtils::reversed(a), ::testing::ElementsAre(3,2,4,1,5));
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    EXPECT_THAT(pht::ListUtils::reversed(a), ::testing::ElementsAre(0,0,3,3,2,3,3,3,2,1));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    EXPECT_THAT(pht::ListUtils::reversed(a), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ReplaceTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    pht::ListUtils::replace(a, 5u, 7u);
    EXPECT_THAT(a, ::testing::ElementsAre(7,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    pht::ListUtils::replace(a, 2u, 7u);
    EXPECT_THAT(a, ::testing::ElementsAre(1,7,3,3,3,7,3,3,0,0));
    a = {1,2,3,3,3,2,3,3,0,0};
    pht::ListUtils::replace(a, 5u, 7u);
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    pht::ListUtils::replace(a, 5u, 7u);
    EXPECT_THAT(a, ::testing::ElementsAre());
}

TEST(ListUtilsTest, ReplacedTest) {
    std::vector<uint32_t> a = {5,1,4,2,3};
    EXPECT_THAT(pht::ListUtils::replaced(a, 5u, 7u), ::testing::ElementsAre(7,1,4,2,3));
    EXPECT_THAT(a, ::testing::ElementsAre(5,1,4,2,3));
    a = {1,2,3,3,3,2,3,3,0,0};
    EXPECT_THAT(pht::ListUtils::replaced(a, 2u, 7u), ::testing::ElementsAre(1,7,3,3,3,7,3,3,0,0));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    EXPECT_THAT(pht::ListUtils::replaced(a, 5u, 7u), ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    EXPECT_THAT(a, ::testing::ElementsAre(1,2,3,3,3,2,3,3,0,0));
    a = {};
    EXPECT_THAT(pht::ListUtils::replaced(a, 5u, 7u), ::testing::ElementsAre());
    EXPECT_THAT(a, ::testing::ElementsAre());
}