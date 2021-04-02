#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#define PHT_TEST

#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/bitvector_utils.h"

#define convertToBitvector pht::Bitvector_Utils::convertToBitvector

class HypersuccinctTreeTest : public ::testing::Test {
protected:
    std::shared_ptr<pht::UnorderedTree<std::string>> treeNath;
    pht::HypersuccinctTree<std::string> hyperNath;
    std::shared_ptr<pht::UnorderedTree<char>> example = std::make_shared<pht::UnorderedTree<char>>();
    std::shared_ptr<pht::Node<char>> a = std::make_shared<pht::Node<char>>('a');
    std::shared_ptr<pht::Node<char>> b = std::make_shared<pht::Node<char>>('b');
    std::shared_ptr<pht::Node<char>> c = std::make_shared<pht::Node<char>>('c');
    std::shared_ptr<pht::Node<char>> d = std::make_shared<pht::Node<char>>('d');
    std::shared_ptr<pht::Node<char>> e = std::make_shared<pht::Node<char>>('e');
    std::shared_ptr<pht::Node<char>> f = std::make_shared<pht::Node<char>>('f');
    std::shared_ptr<pht::Node<char>> g = std::make_shared<pht::Node<char>>('g');
    std::shared_ptr<pht::Node<char>> h = std::make_shared<pht::Node<char>>('h');
    std::shared_ptr<pht::Node<char>> i = std::make_shared<pht::Node<char>>('i');
    std::shared_ptr<pht::Node<char>> j = std::make_shared<pht::Node<char>>('j');
    std::shared_ptr<pht::Node<char>> k = std::make_shared<pht::Node<char>>('k');
    std::shared_ptr<pht::Node<char>> l = std::make_shared<pht::Node<char>>('l');
    std::shared_ptr<pht::Node<char>> m = std::make_shared<pht::Node<char>>('m');
    std::shared_ptr<pht::Node<char>> n = std::make_shared<pht::Node<char>>('n');
    std::shared_ptr<pht::Node<char>> o = std::make_shared<pht::Node<char>>('o');
    std::shared_ptr<pht::Node<char>> p = std::make_shared<pht::Node<char>>('p');
    std::shared_ptr<pht::Node<char>> q = std::make_shared<pht::Node<char>>('q');
    std::shared_ptr<pht::Node<char>> r = std::make_shared<pht::Node<char>>('r');
    std::shared_ptr<pht::Node<char>> s = std::make_shared<pht::Node<char>>('s');
    std::shared_ptr<pht::Node<char>> t = std::make_shared<pht::Node<char>>('t');
    std::shared_ptr<pht::Node<char>> u = std::make_shared<pht::Node<char>>('u');
    std::shared_ptr<pht::Node<char>> v = std::make_shared<pht::Node<char>>('v');
    std::shared_ptr<pht::Node<char>> w = std::make_shared<pht::Node<char>>('w');
    std::shared_ptr<pht::Node<char>> x = std::make_shared<pht::Node<char>>('x');
    std::shared_ptr<pht::Node<char>> y = std::make_shared<pht::Node<char>>('y');
    std::shared_ptr<pht::Node<char>> z = std::make_shared<pht::Node<char>>('z');
    std::shared_ptr<pht::Node<char>> A = std::make_shared<pht::Node<char>>('A');
    std::shared_ptr<pht::Node<char>> B = std::make_shared<pht::Node<char>>('B');

    void SetUp() override {
        example->add(a);
        example->add(b, a);
        example->add(c, a);
        example->add(d, a);
        example->add(e, a);
        example->add(f, a);
        example->add(g, b);
        example->add(h, b);
        example->add(i, b);
        example->add(j, b);
        example->add(k, d);
        example->add(l, g);
        example->add(m, i);
        example->add(n, k);
        example->add(o, k);
        example->add(p, k);
        example->add(q, o);
        example->add(r, o);
        example->add(s, q);
        example->add(t, q);
        example->add(u, q);
        example->add(v, q);
        example->add(w, q);
        example->add(x, r);
        example->add(y, r);
        example->add(z, r);
        example->add(A, t);
        example->add(B, x);

        treeNath  = pht::XMLReader::readByName("treeNath.xml");
        hyperNath = pht::HypersuccinctTreeFactory::create(treeNath);

    }

};

TEST_F(HypersuccinctTreeTest, TreeDataTest) {
    EXPECT_EQ(77, treeNath->getSize());
    EXPECT_EQ("((((((((24)23,(26)25)22)21)20,((29,30,31,32)28,(34,35)33)27)19,(37,38)36)18,(((42,43,44)41,(46,47)45,(((51,52,53)50,(55,56)54)49,(58,59)57,(61,(63,64,65,66)62,(68)67)60,(70)69)48)40)39)17,((((14,15)13)12,16)11,((((7,8)6,(10)9)5)4)3)2,((73,74)72,(76,77)75)71)1;",
              treeNath->toNewickString());
    pht::Bitvector miniSize = convertToBitvector("1100");
    pht::Bitvector microSize = convertToBitvector("100");
    EXPECT_EQ(miniSize, hyperNath.getMiniSize());
    EXPECT_EQ(microSize, hyperNath.getMicroSize());
}

TEST_F(HypersuccinctTreeTest, MiniTreesTest){
    pht::Bitvector miniFIDs = convertToBitvector("0111110101001011010111101010001001001");
    pht::Bitvector miniConnectionTypeVectors = convertToBitvector("001100011111");
    pht::Bitvector miniDummys = convertToBitvector("0000000000000000000000100000000000000000");
    EXPECT_EQ(8, hyperNath.getMiniTrees().size());
    EXPECT_EQ(miniFIDs, hyperNath.getMiniFIDs());
    EXPECT_EQ(miniConnectionTypeVectors, hyperNath.getminiTypeVectors());
    EXPECT_EQ(miniDummys, hyperNath.getMiniDummys());
}

TEST_F(HypersuccinctTreeTest, MicroTreesTest){
    // MiniTree 0
    EXPECT_EQ(hyperNath.getMiniTree(0).microTrees, convertToBitvector("1100011111101001101000"));
    EXPECT_EQ(hyperNath.getMiniTree(0).FIDs, convertToBitvector("1101010"));
    EXPECT_EQ(hyperNath.getMiniTree(0).typeVectors, convertToBitvector("01"));
    EXPECT_EQ(hyperNath.getMiniTree(0).dummys, convertToBitvector("00000000"));

    // MiniTree 1
    EXPECT_EQ(hyperNath.getMiniTree(1).microTrees, convertToBitvector("1100111110000101100001001110100000110111010011000"));
    EXPECT_EQ(hyperNath.getMiniTree(1).FIDs, convertToBitvector("0101111010111101010"));
    EXPECT_EQ(hyperNath.getMiniTree(1).typeVectors, convertToBitvector("0010111"));
    EXPECT_EQ(hyperNath.getMiniTree(1).dummys, convertToBitvector("00000010000000000000"));

    // MiniTree 2
    EXPECT_EQ(hyperNath.getMiniTree(2).microTrees, convertToBitvector("110"));
    EXPECT_EQ(hyperNath.getMiniTree(2).FIDs, convertToBitvector(""));
    EXPECT_EQ(hyperNath.getMiniTree(2).typeVectors, convertToBitvector(""));
    EXPECT_EQ(hyperNath.getMiniTree(2).dummys, convertToBitvector("0000"));

    // MiniTree 3
    EXPECT_EQ(hyperNath.getMiniTree(3).microTrees, convertToBitvector("0010011101000"));
    EXPECT_EQ(hyperNath.getMiniTree(3).FIDs, convertToBitvector("11"));
    EXPECT_EQ(hyperNath.getMiniTree(3).typeVectors, convertToBitvector("1"));
    EXPECT_EQ(hyperNath.getMiniTree(3).dummys, convertToBitvector("0000"));

    // MiniTree 4
    EXPECT_EQ(hyperNath.getMiniTree(4).microTrees, convertToBitvector("1100010111101001000010011010100"));
    EXPECT_EQ(hyperNath.getMiniTree(4).FIDs, convertToBitvector("11011110011100"));
    EXPECT_EQ(hyperNath.getMiniTree(4).typeVectors, convertToBitvector("0011"));
    EXPECT_EQ(hyperNath.getMiniTree(4).dummys, convertToBitvector("000000000000"));

    // MiniTree 5
    EXPECT_EQ(hyperNath.getMiniTree(5).microTrees, convertToBitvector("1100111110000010011101000001011101010100001011110011000"));
    EXPECT_EQ(hyperNath.getMiniTree(5).FIDs, convertToBitvector("01011110101100100100001010"));
    EXPECT_EQ(hyperNath.getMiniTree(5).typeVectors, convertToBitvector("0010111"));
    EXPECT_EQ(hyperNath.getMiniTree(5).dummys, convertToBitvector("00000010000000000000"));

    // MiniTree 6
    EXPECT_EQ(hyperNath.getMiniTree(6).microTrees, convertToBitvector("0010011101000001001110100000100110110000010011010100001011101010100"));
    EXPECT_EQ(hyperNath.getMiniTree(6).FIDs, convertToBitvector("01111101011011110011100001001000"));
    EXPECT_EQ(hyperNath.getMiniTree(6).typeVectors, convertToBitvector("010011011"));
    EXPECT_EQ(hyperNath.getMiniTree(6).dummys, convertToBitvector("00000000000000000000"));

    // MiniTree 7
    EXPECT_EQ(hyperNath.getMiniTree(7).microTrees, convertToBitvector("011111000"));
    EXPECT_EQ(hyperNath.getMiniTree(7).FIDs, convertToBitvector("11"));
    EXPECT_EQ(hyperNath.getMiniTree(7).typeVectors, convertToBitvector("1"));
    EXPECT_EQ(hyperNath.getMiniTree(7).dummys, convertToBitvector("0000"));

}

TEST_F(HypersuccinctTreeTest, CreateViaFactoryAlexTest) {
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = pht::XMLReader::readByName("treeAlex.xml");
    pht::HypersuccinctTree<std::string> hst = pht::HypersuccinctTreeFactory::create(xmlTree);

    EXPECT_THAT(hst.getMiniSize(), ::testing::ElementsAre(1,1,0,0));
    EXPECT_THAT(hst.getMicroSize(), ::testing::ElementsAre(1,0,0));

    EXPECT_EQ(hst.getMiniTrees().size(), 9);
    EXPECT_THAT(hst.getMiniFIDs(), ::testing::ElementsAre(0,1,0,1,1,1,1,0,1,0,1,1,0,1,0,1,0,1,1,0,1,1,1,0,0,0,1,0,1,0,0,1,1,1,0,1));
    EXPECT_THAT(hst.getminiTypeVectors(), ::testing::ElementsAre(0,0,1,0,0,1,0,1,1,1,1));
    EXPECT_THAT(hst.getMiniDummys(), ::testing::ElementsAre(0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));

    pht::MiniTree miniTree = hst.getMiniTree(0);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(1,1,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre());
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre());
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,0,0));

    miniTree = hst.getMiniTree(1);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(0,0,1,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,1,1,0,0,1,1,1,0,0,0,0,0,0,1,0,0,1,1,0,1,0,1,0,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre(1,1,0,1,1,1,0,0,0,1,1,1,0,0));
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre(1,1,1));
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,1,1,0,1,1,0,0,0,0,0));

    miniTree = hst.getMiniTree(2);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(1,1,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre());
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre());
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,0,0));

    miniTree = hst.getMiniTree(3);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(0,0,1,0,1,1,1,0,1,1,0,1,0,0,0,0,0,1,1,0,1,1,0,1,1,0,0,1,1,0,0,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre(0,1,0,1,0,0,1,1,1,0,0));
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre(1,1));
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,1,1,0,0,0,0));

    miniTree = hst.getMiniTree(4);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(1,1,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre());
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre());
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,0,0));

    miniTree = hst.getMiniTree(5);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(0,0,1,0,0,1,1,1,0,0,1,0,0,0,0,1,1,0,1,1,0,1,1,1,0,0,1,0,0,0,0,0,1,0,0,1,1,0,1,0,1,0,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre(0,1,1,1,0,1,0,1,0,1,0,0,1,1,1,0,0));
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre(1,0,1,1));
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,0,0,0,1,0,1,0,0,0,0));

    miniTree = hst.getMiniTree(6);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(1,1,0,1,1,0,0,0,1,0,0,1,1,0,1,0,1,0,0,0,0,1,1,0,1,1,0,1,0,1,1,0,1,0,0,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre(0,1,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,0,0));
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre(0,0,0,1,1));
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));

    miniTree = hst.getMiniTree(7);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(1,1,0,0,1,0,1,1,0,0,0,0,1,1,1,1,1,1,0,1,0,0,1,1,0,1,0,0,0,0,0,1,0,0,1,1,0,1,0,1,0,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre(0,1,0,1,1,0,1,0,1,1,0,1,0,1,0,0,1,1,1,0,0));
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre(0,0,0,1,1,1));
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));

    miniTree = hst.getMiniTree(8);
    EXPECT_THAT(miniTree.microTrees, ::testing::ElementsAre(0,1,0,1,1,0,0));
    EXPECT_THAT(miniTree.FIDs, ::testing::ElementsAre(1,1));
    EXPECT_THAT(miniTree.typeVectors, ::testing::ElementsAre(1));
    EXPECT_THAT(miniTree.dummys, ::testing::ElementsAre(0,0,0,0));
}