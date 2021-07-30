//
// Created by User on 30.07.2021.
//
#define PHT_TEST
#define MEASURE_TIME(func, output) timer.start(); func; timer.stop(); output.emplace_back(name,timer.toString());

#include "pht/xml_reader.h"
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/bitvector_utils.h"
#include "pht/logger.h"
#include <fstream>

#define convertToBitvector pht::Bitvector_Utils::convertToBitvector

int main() {
    //Laufzeittests:
    /*
     * Queries
     * Factory
     * WriteToFile / ReadFromFile
     */
    std::vector<std::string> fileNames = {"treeNath.xml"};
    std::vector<pair<std::string& ,std::string&>> factoryTimes;
    std::vector<pair<std::string& ,std::string&>> childTimes;
    std::vector<pair<std::string& ,std::string&>> degreeTimes;
    std::vector<pair<std::string& ,std::string&>> leftmost_leafTimes;
    std::vector<pair<std::string& ,std::string&>> rightmost_leafTimes;

    std::vector<pair<std::string& ,std::string&>> childRankTimes;
    std::vector<pair<std::string& ,std::string&>> subTreeTimes;
    std::vector<pair<std::string& ,std::string&>> depthTimes;
    std::vector<pair<std::string& ,std::string&>> heightTimes;
    std::vector<pair<std::string& ,std::string&>> isDummyAncestorWithinMiniTreeTimes;
    std::vector<pair<std::string& ,std::string&>> isDummyAncestorWithinMicroTreeTimes;
    std::vector<pair<std::string& ,std::string&>> leafSizeTimes;
    std::vector<pair<std::string& ,std::string&>> leafRankTimes;
    pht::Timer timer;
    for(std::string &name : fileNames) {
        std::shared_ptr<pht::UnorderedTree<std::string>> tree  = pht::XMLReader::readByName(name);
        timer.start();
        pht::HypersuccinctTree hyperTree = pht::HypersuccinctTreeFactory::create(tree);
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
            //pht::HstNode child = hyperTree.child({0,0,0},i);
            timer.stop();
            childTimes.emplace_back(name,timer.toString());
            //testNodes.push_back(child);
        }

        timer.start();
        uint32_t rootSubSize = hyperTree.subtree_size({0,0,0});
        timer.stop();
        subTreeTimes.emplace_back(name,timer.toString());
        uint32_t testSize = 1001;
        if(rootSubSize < testSize) {
            testSize = rootSubSize;
        }

        while(testNodes.size() < testSize) {
            uint32_t i=0;
            uint32_t degree;
            timer.start();
            degree = hyperTree.degree(testNodes.at(i));
            timer.stop();
            degreeTimes.emplace_back(name,timer.toString());
            for(uint32_t j = 0; j<degree; j++) {
                timer.start();
                //pht::HstNode child = hyperTree.child(testNodes.at(i),j);
                timer.stop();
                childTimes.emplace_back(name,timer.toString());
                //testNodes.push_back(child);
            }
            i++;
        }

        for(pht::HstNode &node : testNodes) {
            timer.start();
            pht::HstNode left_leaf = hyperTree.leftmost_leaf(node);
            timer.stop();
            leftmost_leafTimes.emplace_back(name,timer.toString());
            if(rootSubSize < 1001) {
                if (!pht::ListUtils::containsAny(testNodes, {left_leaf})) {
                    testNodes.push_back(left_leaf);
                }
            }

            timer.start();
            pht::HstNode right_leaf = hyperTree.rightmost_leaf(node);
            timer.stop();
            rightmost_leafTimes.emplace_back(name,timer.toString());
            if(rootSubSize < 1001) {
                if (!pht::ListUtils::containsAny(testNodes, {right_leaf})) {
                    testNodes.push_back(right_leaf);
                }
            }
        }

        for(pht::HstNode &node : testNodes) {
            MEASURE_TIME(hyperTree.child_rank(node),childRankTimes);
            MEASURE_TIME(hyperTree.subtree_size(node),subTreeTimes);
            MEASURE_TIME(hyperTree.depth(node),depthTimes);
            MEASURE_TIME(hyperTree.height(node),heightTimes);
            MEASURE_TIME(hyperTree.isDummyAncestorWithinMiniTree(node),isDummyAncestorWithinMiniTreeTimes);
            MEASURE_TIME(hyperTree.isDummyAncestorWithinMicroTree(node),isDummyAncestorWithinMicroTreeTimes);
            MEASURE_TIME(hyperTree.leaf_size(node),leafSizeTimes);
            MEASURE_TIME(hyperTree.leaf_rank(node),leafRankTimes);
        }
    }

    std::ofstream file;
    file.open("testResults.csv");
    file << "Tree, TestName, Result\n";
    for(std::pair<std::string&,std::string&> value : factoryTimes) {
        file << value.first << ", Factory::create, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : childTimes) {
        file << value.first << ", child, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : degreeTimes) {
        file << value.first << ", degree, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : leftmost_leafTimes) {
        file << value.first << ", leftmost_leaf, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : rightmost_leafTimes) {
        file << value.first << ", rightmost_leaf, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : childRankTimes) {
        file << value.first << ", child_rank, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : subTreeTimes) {
        file << value.first << ", subtree_size, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : depthTimes) {
        file << value.first << ", depth, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : heightTimes) {
        file << value.first << ", height, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : isDummyAncestorWithinMiniTreeTimes) {
        file << value.first << ", isDummyAncestorWithinMiniTree, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : isDummyAncestorWithinMicroTreeTimes) {
        file << value.first << ", isDummyAncestorWithinMicroTree, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : leafSizeTimes) {
        file << value.first << ", leaf_size, " << value.second << "\n";
    }
    for(std::pair<std::string&,std::string&> value : leafRankTimes) {
        file << value.first << ", leaf_rank, " << value.second << "\n";
    }
    file.close();
}