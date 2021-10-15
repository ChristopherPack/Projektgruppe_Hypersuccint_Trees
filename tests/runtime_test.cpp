//
// Created by Christopher Pack on 30.07.2021.
//
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"


//#define PHT_TEST
#define MEASURE_TIME(func, output) timer.start(); func; timer.stop(); output.emplace_back(name,timer.toString());

#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/bitvector_utils.h"
#include "pht/logger.h"
#include <fstream>
#include <filesystem>

#define convertToBitvector pht::Bitvector_Utils::convertToBitvector

class RuntimeTest : public ::testing::Test {
protected:
    //Laufzeittests:
    /*
     * Queries
     * Factory
     * WriteToFile / ReadFromFile
     */
    std::vector<std::string> fileNames = {"DBLP.xml"};
    std::vector<pair<std::string , std::string >> factoryTimes;
    std::vector<pair<std::string , std::string >> childTimes;
    std::vector<pair<std::string , std::string >> degreeTimes;
    std::vector<pair<std::string , std::string >> leftmost_leafTimes;
    std::vector<pair<std::string , std::string >> rightmost_leafTimes;

    std::vector<pair<std::string , std::string >> childRankTimes;
    std::vector<pair<std::string , std::string >> subTreeTimes;
    std::vector<pair<std::string , std::string >> depthTimes;
    std::vector<pair<std::string , std::string >> heightTimes;
    std::vector<pair<std::string , std::string >> isDummyAncestorWithinMiniTreeTimes;
    std::vector<pair<std::string , std::string >> isDummyAncestorWithinMicroTreeTimes;
    std::vector<pair<std::string , std::string >> leafSizeTimes;
    std::vector<pair<std::string , std::string >> leafRankTimes;
    pht::Timer timer;
};

TEST_F(RuntimeTest, MiniTreesTest) {
    for(std::string &name : fileNames) {
        PHT_LOGGER_INFO("Runtime Test", "Begin: " + name);
        std::cout << "Reading " + name << std::endl;
        std::shared_ptr<pht::UnorderedTree<std::string>> tree = pht::XMLReader::readByName(name);
        PHT_LOGGER_INFO("Runtime Test", "Finished Reading File.");
        std::cout << "Finished reading " + name << std::endl;
        std::cout << "Nodes: " + std::to_string(tree->getSize()) << std::endl;
        timer.start();
        pht::HypersuccinctTree hyperTree = *pht::HypersuccinctTreeFactory::create(tree, false, 0, 0);
        timer.stop();
        tree.reset();
        factoryTimes.emplace_back(name, timer.toString());

        std::vector<pht::HstNode > testNodes = {{0,0,0}};
        uint32_t rootdegree;
        timer.start();
        rootdegree = hyperTree.degree({0,0,0});
        timer.stop();
        degreeTimes.emplace_back(name,timer.toString());
        for(uint32_t i = 0; i<rootdegree; i++) {
            timer.start();
            pht::HstNode child = hyperTree.child({0,0,0},i);
            timer.stop();
            childTimes.emplace_back(name,timer.toString());
            testNodes.push_back(child);
        }

        timer.start();
        uint32_t rootSubSize = hyperTree.subtreeSize({0,0,0});
        timer.stop();
        subTreeTimes.emplace_back(name,timer.toString());
        uint32_t testSize = 1001;
        if(rootSubSize < testSize) {
            testSize = rootSubSize;
        }

        uint32_t i=1;
        while(testNodes.size() < testSize) {
            if(i >= testNodes.size()) {
                break;
            }
            uint32_t degree;
            timer.start();
            degree = hyperTree.degree(testNodes.at(i));
            timer.stop();
            degreeTimes.emplace_back(name,timer.toString());
            for(uint32_t j = 0; j<degree; j++) {
                if(testNodes.size() >= testSize) {
                    break;
                }
                timer.start();
                pht::HstNode child = hyperTree.child(testNodes.at(i),j);
                timer.stop();
                childTimes.emplace_back(name,timer.toString());
                if(testNodes.size() < testSize) {
                    testNodes.push_back(child);
                }
            }
            i++;
        }


        for(pht::HstNode &node : testNodes) {
            timer.start();
            pht::HstNode left_leaf = hyperTree.leftmostLeaf(node);
            timer.stop();
            leftmost_leafTimes.emplace_back(name,timer.toString());
            if(rootSubSize < 1001) {
                if (!pht::ListUtils::containsAny(testNodes, {left_leaf})) {
                    testNodes.push_back(left_leaf);
                }
            }

            timer.start();
            pht::HstNode right_leaf = hyperTree.rightmostLeaf(node);
            timer.stop();
            rightmost_leafTimes.emplace_back(name,timer.toString());
            if(rootSubSize < 1001) {
                if (!pht::ListUtils::containsAny(testNodes, {right_leaf})) {
                    testNodes.push_back(right_leaf);
                }
            }
        }

        for(pht::HstNode &node : testNodes) {
            MEASURE_TIME(hyperTree.childRank(node),childRankTimes);
            MEASURE_TIME(hyperTree.subtreeSize(node),subTreeTimes);
            MEASURE_TIME(hyperTree.depth(node),depthTimes);
            MEASURE_TIME(hyperTree.height(node),heightTimes);
            MEASURE_TIME(hyperTree.isDummyAncestorWithinMiniTree(node),isDummyAncestorWithinMiniTreeTimes);
            MEASURE_TIME(hyperTree.isDummyAncestorWithinMicroTree(node),isDummyAncestorWithinMicroTreeTimes);
            MEASURE_TIME(hyperTree.leafSize(node),leafSizeTimes);
            MEASURE_TIME(hyperTree.leafRank(node),leafRankTimes);
        }

        testNodes.clear();
    }


    PHT_LOGGER_INFO("Runtime Test", "Begin File Output");
    std::ofstream file;
    file.open("testResults.csv");
    file << "Tree, TestName, Result\n";
    for(std::pair<std::string,std::string> &value : factoryTimes) {
        file << value.first << ", Factory::create, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : childTimes) {
        file << value.first << ", child, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : degreeTimes) {
        file << value.first << ", degree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leftmost_leafTimes) {
        file << value.first << ", leftmost_leaf, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : rightmost_leafTimes) {
        file << value.first << ", rightmost_leaf, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : childRankTimes) {
        file << value.first << ", child_rank, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : subTreeTimes) {
        file << value.first << ", subtree_size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : depthTimes) {
        file << value.first << ", depth, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : heightTimes) {
        file << value.first << ", height, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : isDummyAncestorWithinMiniTreeTimes) {
        file << value.first << ", isDummyAncestorWithinMiniTree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : isDummyAncestorWithinMicroTreeTimes) {
        file << value.first << ", isDummyAncestorWithinMicroTree, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leafSizeTimes) {
        file << value.first << ", leaf_size, " << value.second << "\n";
    }
    for(std::pair<std::string,std::string> &value : leafRankTimes) {
        file << value.first << ", leaf_rank, " << value.second << "\n";
    }
    file.close();
}