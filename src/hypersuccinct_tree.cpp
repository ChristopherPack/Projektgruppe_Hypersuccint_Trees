//
// Created by User on 23.04.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>


using namespace pht;

bool HypersuccinctTree::isDummyAncestorWithinMiniTree(HstNode node) {
    //assert(std::get<1>(node) <= Bitvector_Utils::getEntryCount(miniTrees.at(std::get<0>(node)).microTrees.cbegin(),miniTrees.at(std::get<0>(node)).microTrees.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {2, 0, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator}));
    MiniTree mini = getMiniTree(std::get<0>(node));
    assert(!mini.rootAncestors.empty());
    HstNode dummy = {std::get<0>(node), Bitvector_Utils::decodeNumber(mini.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) , Bitvector_Utils::decodeNumber(mini.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY)};
    Bitvector micro = getMicroTree(mini,std::get<1>(dummy));
    assert(std::get<2>(dummy) <= (getMicroTree(mini, std::get<1>(dummy)).size()/2));
    if(std::get<2>(node) == 0) {
        //explicitly stored Root is Ancestor
        Bitvector rootAnc = mini.rootAncestors;
        if(rootAnc.at(std::get<1>(node))) {
            return true;
        }
        return false;
    }
    else if(std::get<1>(node) == std::get<1>(dummy)) {
        //lookupTable
        Bitvector index = getMicroTree(mini, std::get<1>(node));
        return lookupTableMatrixComparison(getLookupTableEntry(index), std::get<2>(node),std::get<2>(dummy));
    }
    else {
        //explicitly stored - microTreeDummyNode is Ancestor of MiniDummy
        Bitvector dummyAnc = mini.dummyAncestors;
        if(dummyAnc.at(std::get<1>(node))) {
            return true;
        }
        return false;
    }
    return false;
}