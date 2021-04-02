#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"

#define PHT_TEST

#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"

TEST(HypersuccinctTreeTest, CreateViaFactoryAlexTest) {
    std::shared_ptr<pht::UnorderedTree<std::string>> xmlTree = pht::XMLReader::read("treeAlex.xml");
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