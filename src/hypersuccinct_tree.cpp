//
// Created by User on 23.04.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>


using namespace pht;

bool HypersuccinctTree::isDummyAncestorWithinMiniTree(HstNode node, HstNode dummy) {
    assert(std::get<0>(node) == std::get<0>(dummy));
    if(std::get<2>(node) == 0) {
        //explicitly stored Root is Ancestor
    }
    else if(std::get<1>(node) == std::get<1>(dummy)) {
        //lookupTable
    }
    else {
        //explicitly stored - microTreeDummyNode is Ancestor of MiniDummy
    }
    return false;
}