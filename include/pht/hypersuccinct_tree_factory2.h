//
// Created by User on 19.02.2021.
//

#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY2_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY2_H

#include <iostream>
#include <iterator>
#include <set>

#include "hypersuccinct_tree.h"
#include "bitvector_utils.h"

/**
 *  ALERT: THIS FILE NEEDS TO BE DELETED LATER
 *  REPEAT: THIS FILE IS NOT PART OF THE PROGRAM; JUST A HELP FOR PROGRAMMING
 *  TODO: DELETE LATER
 */
namespace pht {
    /**
     * Creates Hypersuccinct Code for a given Unordered Tree utilizing the Farzan Munro Algorithm.
     */
    class HypersuccinctTreeFactory2 {
    public:
        /**
         * Creates Hypersuccinct Code for a given Unordered Tree utilizing the Farzan Munro Algorithm.
         * todo: reevaluate Function Structure
         *
         * @tparam T Class implemented in UnorderedTree.
         * @param tree the UnorderedTree to be encoded.
         * @return HypersuccinctTree class representing the Hypersuccinct code todo: see Class
         */
        template<class T> static HypersuccinctTree<T> create(const std::shared_ptr<UnorderedTree<T>> tree) {
            HypersuccinctTree<T> hypersuccinctTree;
            uint32_t size = ceil(pow(log2(tree->getSize()), 2.0));
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMiniTrees = pht::ListUtils::reverse(pht::FarzanMunro<T>::decompose(tree, size));
            size = ceil((log2(tree->getSize()))/8.0);


            for(std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree : fmMiniTrees) {

                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees = pht::ListUtils::reverse(pht::FarzanMunro<T>::decompose(fmMiniTree, size));
                MiniTree miniTree = MiniTree();
                for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                    std::vector<bool> bp = fmMicroTree->toBalancedParenthesis();
                    MicroTreeData microTreeData(bp);
                    if(!ListUtils::contains(hypersuccinctTree.lookupTable, microTreeData)) {
                        hypersuccinctTree.lookupTable.push_back(microTreeData);
                    }
                }
                //BITVECTOR FUNCTION
                miniTree.microTrees = Bitvector_Utils::createBitVectorforMicroTrees(fmMicroTrees);
                hypersuccinctTree.miniTrees.push_back(miniTree);

                //hypersuccinctTree.miniTrees.push_back(createMiniTree(fmMiniTree,size,hypersuccinctTree));
            }

            //todo: create Interconnections MicroTrees

            //todo: create Interconnections MiniTrees

            return hypersuccinctTree;
        }

        /**
         * Hilfsmethode zur Erzeugung von Hypersuccinct Code.
         *
         * @tparam T
         * @param fmMiniTree
         * @param size
         * @param hypersuccinctTree
         * @return
         */
        template<class T> static MiniTree createMiniTree(std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree,uint32_t size,HypersuccinctTree<T> hypersuccinctTree) {
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees = pht::ListUtils::reverse(pht::FarzanMunro<T>::decompose(fmMiniTree, size));
            MiniTree miniTree = MiniTree();
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                std::vector<bool> bp = fmMicroTree->toBalancedParenthesis();
                MicroTreeData microTreeData(bp);
                if(!ListUtils::contains(hypersuccinctTree.lookupTable, microTreeData)) {
                    hypersuccinctTree.lookupTable.push_back(microTreeData);
                }
            }
            //BITVECTOR FUNCTION
            miniTree.microTrees = Bitvector_Utils::createBitVectorforMicroTrees(fmMicroTrees);
        }

        /*template<class T> static ?? createInterconnections() {

        }

        template<class T> static Bitvector createType3Interconnections() {

        }

        template<class T> static Bitvector createType12Interconnections() {

        }*/


    };
}


#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY2_H
