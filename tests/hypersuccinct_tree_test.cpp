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
    std::shared_ptr<pht::UnorderedTree<std::string>> treeNath  = pht::XMLReader::readByName("treeNath.xml");
    pht::HypersuccinctTree hyperNath = *pht::HypersuccinctTreeFactory::create(treeNath, false, 12, 4);
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




    }

};

TEST_F(HypersuccinctTreeTest, MiniTreesTest){
    std::vector<pht::Bitvector> miniFIDs = {{true,true,true},{true,false},{true,true},{true,true},{true},{true,false},{true,false,false,true}};
    std::vector<pht::Bitvector> miniConnectionTypeVectors = {{0,0,1},{1},{0,0},{0,1},{1},{1},{1,1}};
    std::vector<pht::Bitvector> miniDummys = {{0},{0},{0},{0},{1,0,0},{0},{0},{0}};
    EXPECT_EQ(8, hyperNath.getMiniTrees().size());
    EXPECT_EQ(miniFIDs, hyperNath.getMiniFIDs());
    EXPECT_EQ(miniConnectionTypeVectors, hyperNath.getMiniTypeVectors());
    EXPECT_EQ(miniDummys, hyperNath.getMiniDummys());
}

TEST_F(HypersuccinctTreeTest, SupportTest) {
    try {
        pht::MiniTree miniTree = hyperNath.getMiniTree(1);
        miniTree.FIDsSupport.at(0).Rank(0);
        miniTree.FIDsSupport.at(0).Select(0);
    }
    catch (runtime_error) {
        EXPECT_EQ(0,1);
    }
}

TEST_F(HypersuccinctTreeTest, getMicroTreeTest) {
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = pht::XMLReader::readByName("treeNath.xml");
    pht::HypersuccinctTree hst = *pht::HypersuccinctTreeFactory::create(xmlTree,true, 12, 4);
    pht::MiniTree mini = hst.getMiniTree(4);
    std::vector<bool> res = hst.getMicroTree(mini, 0);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0));
    res = hst.getMicroTree(mini, 1);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1,0,1,0,0,1,0,0));
    res = hst.getMicroTree(mini, 2);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,0,1,0,1,0,0));
}

TEST_F(HypersuccinctTreeTest, getFIDforMiniTreeTest) {
    std::vector<bool> res = hyperNath.getFIDforMiniTree(0);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1,1));
    res = hyperNath.getFIDforMiniTree(1);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0));
    res = hyperNath.getFIDforMiniTree(2);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1));
    res = hyperNath.getFIDforMiniTree(3);
    EXPECT_THAT(res, ::testing::ElementsAre(1,1));
    res = hyperNath.getFIDforMiniTree(4);
    EXPECT_THAT(res, ::testing::ElementsAre(1));
    res = hyperNath.getFIDforMiniTree(5);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0));
    res = hyperNath.getFIDforMiniTree(6);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,0,1));
    res = hyperNath.getFIDforMiniTree(7);
    EXPECT_THAT(res, ::testing::ElementsAre(1,0,0,1));
}

TEST_F(HypersuccinctTreeTest, getTreesForFIDTest) {
    std::tuple<std::vector<uint32_t >,std::vector<uint32_t >> res = hyperNath.getTreesForFID(0);
    EXPECT_THAT(std::get<0>(res), ::testing::ElementsAre(0));
    EXPECT_THAT(std::get<1>(res), ::testing::ElementsAre(1,2));

    res = hyperNath.getTreesForFID(1);
    EXPECT_THAT(std::get<0>(res), ::testing::ElementsAre(1));
    EXPECT_THAT(std::get<1>(res), ::testing::ElementsAre());

    res = hyperNath.getTreesForFID(2);
    EXPECT_THAT(std::get<0>(res), ::testing::ElementsAre(2));
    EXPECT_THAT(std::get<1>(res), ::testing::ElementsAre(3,4));

    res = hyperNath.getTreesForFID(3);
    EXPECT_THAT(std::get<0>(res), ::testing::ElementsAre(3));
    EXPECT_THAT(std::get<1>(res), ::testing::ElementsAre(5));

    res = hyperNath.getTreesForFID(4);
    EXPECT_THAT(std::get<0>(res), ::testing::ElementsAre(4));
    EXPECT_THAT(std::get<1>(res), ::testing::ElementsAre());

    res = hyperNath.getTreesForFID(5);
    EXPECT_THAT(std::get<0>(res), ::testing::ElementsAre(5));
    EXPECT_THAT(std::get<1>(res), ::testing::ElementsAre());

    res = hyperNath.getTreesForFID(6);
    EXPECT_THAT(std::get<0>(res), ::testing::ElementsAre(6,7));
    EXPECT_THAT(std::get<1>(res), ::testing::ElementsAre());
}

TEST_F(HypersuccinctTreeTest, getTreesForMicroFIDTest) {
    pht::MiniTree miniTree = hyperNath.getMiniTree(4);
    std::tuple<std::vector<uint32_t >,std::vector<uint32_t >> res = hyperNath.getTreesForMicroFID(miniTree, 0);
    EXPECT_THAT(std::get<0>(res), ::testing::ElementsAre(0));
    EXPECT_THAT(std::get<1>(res), ::testing::ElementsAre(1));
}

TEST_F(HypersuccinctTreeTest, TreeToFIDIndexConversionTest) {
    std::tuple<uint32_t,uint32_t> res = hyperNath.convertTreeToFIDIndex(0);
    EXPECT_EQ(std::get<0>(res), 0);
    EXPECT_EQ(std::get<1>(res), -1);

    res = hyperNath.convertTreeToFIDIndex(1);
    EXPECT_EQ(std::get<0>(res), 1);
    EXPECT_EQ(std::get<1>(res), 0);

    res = hyperNath.convertTreeToFIDIndex(2);
    EXPECT_EQ(std::get<0>(res), 2);
    EXPECT_EQ(std::get<1>(res), 0);

    res = hyperNath.convertTreeToFIDIndex(3);
    EXPECT_EQ(std::get<0>(res), 3);
    EXPECT_EQ(std::get<1>(res), 2);

    res = hyperNath.convertTreeToFIDIndex(4);
    EXPECT_EQ(std::get<0>(res), 4);
    EXPECT_EQ(std::get<1>(res), 2);

    res = hyperNath.convertTreeToFIDIndex(5);
    EXPECT_EQ(std::get<0>(res), 5);
    EXPECT_EQ(std::get<1>(res), 3);

    res = hyperNath.convertTreeToFIDIndex(6);
    EXPECT_EQ(std::get<0>(res), 6);
    EXPECT_EQ(std::get<1>(res), -1);

    res = hyperNath.convertTreeToFIDIndex(7);
    EXPECT_EQ(std::get<0>(res), 6);
    EXPECT_EQ(std::get<1>(res), -1);
}

TEST_F(HypersuccinctTreeTest, MicroTreeToFIDIndexConversionTest) {
    pht::MiniTree miniTree = hyperNath.getMiniTree(1);
    std::tuple<uint32_t,uint32_t> res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 0);
    EXPECT_EQ(std::get<0>(res), 0);
    EXPECT_EQ(std::get<1>(res), -1);

    res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 1);
    EXPECT_EQ(std::get<0>(res), 1);
    EXPECT_EQ(std::get<1>(res), 0);

    res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 2);
    EXPECT_EQ(std::get<0>(res), 2);
    EXPECT_EQ(std::get<1>(res), 0);

    res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 3);
    EXPECT_EQ(std::get<0>(res), 3);
    EXPECT_EQ(std::get<1>(res), 2);

    res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 4);
    EXPECT_EQ(std::get<0>(res), 4);
    EXPECT_EQ(std::get<1>(res), -1);

    miniTree = hyperNath.getMiniTree(0);
    res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 1);
    EXPECT_EQ(std::get<0>(res), 1);
    EXPECT_EQ(std::get<1>(res), 0);

    miniTree = hyperNath.getMiniTree(4);
    res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 1);
    EXPECT_EQ(std::get<0>(res), 1);
    EXPECT_EQ(std::get<1>(res), 0);

    miniTree = hyperNath.getMiniTree(0);
    res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 0);
    EXPECT_EQ(std::get<0>(res), 0);
    EXPECT_EQ(std::get<1>(res), -1);

    res = hyperNath.convertMicroTreeToFIDIndex(miniTree, 1);
    EXPECT_EQ(std::get<0>(res), 1);
    EXPECT_EQ(std::get<1>(res), 0);
}

TEST_F(HypersuccinctTreeTest, isDummyAncestorWithinMiniTreeTest) {
    pht::HstNode node = {4,1,1};
    EXPECT_FALSE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,0,0};
    EXPECT_TRUE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,2,0};
    EXPECT_FALSE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,2,2};
    EXPECT_FALSE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,1,2};
    EXPECT_FALSE(hyperNath.isDummyAncestorWithinMiniTree(node));
    node = {4,1,4};
    EXPECT_TRUE(hyperNath.isDummyAncestorWithinMiniTree(node));
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = pht::XMLReader::readByName("treeNath.xml");
    pht::HypersuccinctTree hst = *pht::HypersuccinctTreeFactory::create(xmlTree,true, 12, 4);
    node = {4,0,0};
    EXPECT_TRUE(hst.isDummyAncestorWithinMiniTree(node));
    node = {4,2,0};
    EXPECT_FALSE(hst.isDummyAncestorWithinMiniTree(node));
    node = {4,2,2};
    EXPECT_FALSE(hst.isDummyAncestorWithinMiniTree(node));
    node = {4,1,2};
    EXPECT_FALSE(hst.isDummyAncestorWithinMiniTree(node));
    node = {4,1,4};
    EXPECT_TRUE(hst.isDummyAncestorWithinMiniTree(node));
}

TEST_F(HypersuccinctTreeTest, childTest) {
    pht::HstNode node = {0,0,0};
    pht::HstNode res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,0,0), res);

    node = {1,1,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,1,1), res);

    node = {0,1,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(0,1,1), res);

    node = {4,1,4};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(6,1,0), res);

    node = {4,2,3};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(), res);

    node = {4,1,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(4,2,0), res);

    node = {1,1,2};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,4,1), res);

    node = {1,1,1};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,1,2), res);

    node = {1,2,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,3,0), res);

    node = {1,2,1};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(), res);

    node = {1,4,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(1,4,1), res);

    node = {3,0,1};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(3,0,2), res);

    node = {6,0,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(6,1,0), res);

    node = {7,0,0};
    res = hyperNath.child(node,0);
    EXPECT_EQ(pht::HstNode(6,1,0), res);

    node = {6,0,0};
    res = hyperNath.child(node,1);
    EXPECT_EQ(pht::HstNode(6,0,1), res);

    node = {6,0,0};
    res = hyperNath.child(node,2);
    EXPECT_EQ(pht::HstNode(6,2,0), res);

    node = {6,0,0};
    res = hyperNath.child(node,3);
    EXPECT_EQ(pht::HstNode(7,0,1), res);

    node = {6,0,0};
    res = hyperNath.child(node,4);
    EXPECT_EQ(pht::HstNode(), res);

}

TEST_F(HypersuccinctTreeTest, child_rankTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {1,1,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {0,1,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {4,1,4};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {4,2,3};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);

    node = {4,1,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {1,1,2};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {1,1,1};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {1,2,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(1, res);

    node = {1,2,1};
    res = hyperNath.childRank(node);
    EXPECT_EQ(1, res);

    node = {1,4,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(0, res);

    node = {3,0,1};
    res = hyperNath.childRank(node);
    EXPECT_EQ(1, res);

    node = {6,0,0};
    res = hyperNath.childRank(node);
    EXPECT_EQ(2, res);
}

TEST_F(HypersuccinctTreeTest, degreeTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.degree(node);
    EXPECT_EQ(3, res);

    node = {2,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {7,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(4, res);

    node = {4,1,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(3, res);

    node = {4,1,1};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {4,1,4};
    res = hyperNath.degree(node);
    EXPECT_EQ(4, res);

    node = {1,1,2};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {6,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(4, res);

    node = {2,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {3,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {1,1,1};
    res = hyperNath.degree(node);
    EXPECT_EQ(1, res);

    node = {7,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(4, res);

    node = {0,0,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(3, res);

    node = {0,1,0};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

    node = {0,1,1};
    res = hyperNath.degree(node);
    EXPECT_EQ(2, res);

}

TEST_F(HypersuccinctTreeTest, subtree_sizeTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.subtreeSize(node);
    EXPECT_EQ(77, res);

    node = {1,1,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(8, res);

    node = {0,1,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(7, res);

    node = {4,1,4};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(23, res);

    node = {6,0,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(23, res);

    node = {1,1,2};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(6, res);

    node = {1,4,0};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(6, res);

    node = {1,1,1};
    res = hyperNath.subtreeSize(node);
    EXPECT_EQ(7, res);

}

TEST_F(HypersuccinctTreeTest, depthTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.depth(node);
    EXPECT_EQ(0, res);

    node = {1,1,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(2, res);

    node = {0,1,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(1, res);

    node = {4,1,4};
    res = hyperNath.depth(node);
    EXPECT_EQ(4, res);

    node = {6,0,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(4, res);

    node = {1,1,2};
    res = hyperNath.depth(node);
    EXPECT_EQ(4, res);

    node = {1,1,1};
    res = hyperNath.depth(node);
    EXPECT_EQ(3, res);

    node = {1,4,0};
    res = hyperNath.depth(node);
    EXPECT_EQ(4, res);

}

TEST_F(HypersuccinctTreeTest, heightTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.height(node);
    EXPECT_EQ(8, res);

    node = {1,1,0};
    res = hyperNath.height(node);
    EXPECT_EQ(4, res);

    node = {0,1,0};
    res = hyperNath.height(node);
    EXPECT_EQ(2, res);

    node = {4,1,4};
    res = hyperNath.height(node);
    EXPECT_EQ(3, res);

    node = {6,0,0};
    res = hyperNath.height(node);
    EXPECT_EQ(3, res);

    node = {1,1,2};
    res = hyperNath.height(node);
    EXPECT_EQ(2, res);

    node = {1,4,0};
    res = hyperNath.height(node);
    EXPECT_EQ(2, res);

    node = {1,1,1};
    res = hyperNath.height(node);
    EXPECT_EQ(3, res);

}

TEST_F(HypersuccinctTreeTest, leaf_sizeTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.leafSize(node);
    EXPECT_EQ(39, res);

    node = {1,1,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

    node = {0,1,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(4, res);

    node = {4,1,4};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(14, res);

    node = {6,0,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(14, res);

    node = {4,2,3};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(1, res);

    node = {1,1,2};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

    node = {1,4,0};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

    node = {1,1,1};
    res = hyperNath.leafSize(node);
    EXPECT_EQ(3, res);

}

TEST_F(HypersuccinctTreeTest, rightmost_leafTest) {
    pht::HstNode node = {0,0,0};
    pht::HstNode res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(0,1,6), res);
    //EXPECT_THAT(res, ::testing::ElementsAre(1,0,0));

    node = {1,1,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,5), res);

    node = {0,1,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(0,1,6), res);

    node = {4,1,4};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(7,0,2), res);

    node = {4,2,3};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(4,2,3), res);

    node = {4,1,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(7,0,2), res);

    node = {1,1,2};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,5), res);

    node = {1,1,1};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,5), res);

    node = {1,2,0};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,2,1), res);

    node = {1,2,1};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,2,1), res);

    node = {3,0,1};
    res = hyperNath.rightmostLeaf(node);
    EXPECT_EQ(pht::HstNode(3,0,3), res);

}

TEST_F(HypersuccinctTreeTest,leftmost_leafTest) {
    pht::HstNode node = {0,0,0};
    pht::HstNode res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,2), res);

    node = {1,1,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,2), res);

    node = {0,1,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(0,1,2), res);

    node = {4,1,4};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(6,3,1), res);

    node = {4,2,3};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(4,2,3), res);

    node = {4,1,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(4,2,1), res);

    node = {1,1,2};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,2), res);

    node = {1,1,1};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,4,2), res);

    node = {1,2,0};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,3,2), res);

    node = {1,2,1};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(1,2,1), res);

    node = {3,0,1};
    res = hyperNath.leftmostLeaf(node);
    EXPECT_EQ(pht::HstNode(3,0,2), res);

}

TEST_F(HypersuccinctTreeTest,leaf_rankTest) {
    pht::HstNode node = {0,0,0};
    uint32_t res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {1,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {2,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(6, res);

    node = {3,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(6, res);

    node = {4,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(16, res);

    node = {5,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(6, res);

    node = {6,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(21, res);

    node = {7,0,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(21, res);

    node = {1,1,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {0,1,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(35, res);

    node = {0,1,6};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(39, res);

    node = {4,2,3};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(19, res);

    node = {4,1,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(16, res);

    node = {1,4,5};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(3, res);

    node = {1,1,2};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {1,1,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(0, res);

    node = {1,2,0};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(3, res);

    node = {1,2,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(6, res);

    node = {3,0,1};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(14, res);

    node = {4,1,4};
    res = hyperNath.leafRank(node);
    EXPECT_EQ(21, res);

}