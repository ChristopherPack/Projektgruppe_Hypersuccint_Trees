//
// Created by User on 23.04.2021.
//

#include "pht/hypersuccinct_tree_factory.h"
#include <iostream>


using namespace pht;

void generateIsDummyAncestorWithinMiniTree(HypersuccinctTree & tree);


template<class T> static void generateQueryData(HypersuccinctTree& tree,std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree) {
    //generateIsDummyAncestorWithinMiniTree(tree);
    std::cout << fmMiniTree->getSize();
}

void generateIsDummyAncestorWithinMiniTree(HypersuccinctTree & tree) {
    for(uint32_t i = 0; i<tree.miniTrees.size(); i++) {
        if(Bitvector_Utils::bitvectorToNumber(tree.getMiniDummy(i)) != 0) {
            MiniTree& mini = tree.miniTrees.at(i);
            Bitvector dummy = tree.getMiniDummy(i);
            uint32_t dummyNum = Bitvector_Utils::bitvectorToNumber(dummy);
            /*
             * Microtreesize < dummyNumber
             * >=
             * 53
             * 10 Microtrees 6 nodes
             * 9. MicroTree (54) >= 53
             * 54- 6 -> 53-48 = 5
             * Dummy = 5. Node in 9. MicroTree
             * todo: same roots don't count
             */
            uint32_t size = 0;
            uint32_t microTree = 0;
            for(int j =0 ; j < Bitvector_Utils::getEGEntryCount(mini.microTrees, 2) ; j++) {
                Bitvector bp = tree.getMicroTree(mini,j);
                if(size+(bp.size()/2) >= dummyNum) {
                    dummyNum -= size;
                    microTree = j;
                }
                size+= (bp.size()/2);
            }
            //rootAncestors set microTree to 1
            /*
             *
             */

        }
    }
}

