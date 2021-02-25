//
// Created by User on 19.02.2021.
//

#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY2_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY2_H

#include <iostream>
#include <iterator>
#include <set>

#include "hypersuccinct_tree.h"

namespace pht {
    class HypersuccinctTreeFactory2 {
    public:
        template<class T> static HypersuccinctTree<T> create(const std::shared_ptr<UnorderedTree<T>> tree) {
            HypersuccinctTree<T> hypersuccinctTree;
            uint32_t size = ceil(pow(log2(tree->getSize()), 2.0));
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMiniTrees = pht::ListUtils::reverse(pht::FarzanMunro<T>::decompose(tree, size));
            size = ceil((log2(tree->getSize()))/8.0);
            //nur einen MiniTree auswählen
            //for(std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree : fmMiniTrees) {
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees = pht::ListUtils::reverse(pht::FarzanMunro<T>::decompose(fmMiniTree, size));
                MiniTree miniTree = MiniTree();
                for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                    std::vector<bool> bp = fmMicroTree->toBalancedParenthesis();
                    MicroTreeData microTreeData(bp);
                    if(!ListUtils::contains(hypersuccinctTree.lookupTable, microTreeData)) {
                        hypersuccinctTree.lookupTable.push_back(microTreeData);
                    }
                    //todo: BITVECTOR FUNCTION
                    /*MicroTree microTree = MicroTree(fmMicroTree->getSize(), ListUtils::indexOf(hypersuccinctTree.lookupTable, microTreeData));
                    hypersuccinctTree.microTrees.push_back(microTree);
                    miniTree.microTrees.push_back(microTree);*/
                }
                hypersuccinctTree.miniTrees.push_back(miniTree);
            //}
            return hypersuccinctTree;
        }
    };
}


#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY2_H
