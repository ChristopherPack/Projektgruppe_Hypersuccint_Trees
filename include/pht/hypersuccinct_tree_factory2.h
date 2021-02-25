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
    typedef std::vector<bool> Bitvector;
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
                }
                //BITVECTOR FUNCTION
                miniTree.microTrees = createBitVectorforMicroTrees(fmMicroTrees);
                hypersuccinctTree.miniTrees.push_back(miniTree);
            //}
            return hypersuccinctTree;
        }

        template<class T> static Bitvector createBitVectorforMicroTrees(std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees) {
            //determine size of Bitvector
            Bitvector res;
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                createBitVectorForMicroTree(fmMicroTree);
            }
            return res;
        }

        template<class T> static Bitvector createBitVectorForMicroTree(Bitvector& res,std::shared_ptr<pht::UnorderedTree<T>> microTree) {
            int32_t size = microTree->getSize();
            int32_t logSize = floor((log2(size)));
            //elias gamma code SIZE
            createEliasGamma<T>(res,size);
            //BP FORM in bitform
            Bitvector bp = microTree->toBalancedParenthesis();
            pht::ListUtils::addAll(res,bp);
        }

        template<class T> static Bitvector createEliasGamma(Bitvector& res, const int32_t size) {
            int32_t logSize = floor((log2(size)));
            for(int i=0; i<logSize;i++) {
                res.push_back(false);
            }
            for(int i =0; i<logSize+1;i++) {
                res.push_back((size>>(logSize-i))%2==1);
            }
        }
    };
}


#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY2_H
