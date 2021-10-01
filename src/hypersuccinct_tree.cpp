//
// Created by Christopher Pack on 23.04.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>


using namespace pht;

Bitvector HypersuccinctTree::getMicroTree(MiniTree& miniTree,uint32_t index) {
    if(huffmanFlag) {
        Bitvector huffman = miniTree.microTrees.at(index);
        return std::find_if(lookupTable.begin(),lookupTable.end(), [&huffman](const LookupTableEntry& microTreeData) {return microTreeData.index == huffman;})->bp;
    }
    else {
        return miniTree.microTrees.at(index);
    }
}

uint32_t HypersuccinctTree::getMicroTreeCount(MiniTree& miniTree) {
    return miniTree.microTrees.size();
}

Bitvector HypersuccinctTree::getMicroFID(MiniTree& miniTree,uint32_t index) {
    return miniTree.FIDs.at(index);
}

Bitvector HypersuccinctTree::getMicroTypeVector(MiniTree& miniTree , uint32_t index) {
    return miniTree.typeVectors.at(index);
}

Bitvector HypersuccinctTree::getMicroDummys(MiniTree& miniTree, uint32_t index) {
    return miniTree.dummys.at(index);
}

Bitvector HypersuccinctTree::getMicroDummyPointers(MiniTree& miniTree, uint32_t index) {
    return miniTree.microDummyPointers.at(index);
}

Bitvector HypersuccinctTree::getMiniDummy(uint32_t index) {
    return miniDummys.at(index);
}

LookupTableEntry HypersuccinctTree::getLookupTableEntry(Bitvector indexV) {
    if(huffmanFlag) {
        auto iter = std::find_if(lookupTable.begin(), lookupTable.end(),[&indexV](const LookupTableEntry &microTreeData) {return microTreeData.bp == indexV;});
        if (iter == lookupTable.end()) {

        }
        return *iter;
    } else {
        auto iter = std::find_if(lookupTable.begin(), lookupTable.end(),[&indexV](const LookupTableEntry &microTreeData) {return microTreeData.index == indexV;});
        if (iter == lookupTable.end()) {

        }
        return *iter;
    }
}

bool HypersuccinctTree::lookupTableAncestorMatrixComparison(const LookupTableEntry& entry, uint32_t anc, uint32_t node2Index) {
    uint32_t size = sqrt(entry.ancestorMatrix.size());
    return entry.ancestorMatrix.at(size*anc + node2Index);
}

bool HypersuccinctTree::lookupTableChildMatrixComparison(const LookupTableEntry& entry, uint32_t child, uint32_t node2Index) {
    uint32_t size = sqrt(entry.childMatrix.size());
    return entry.childMatrix.at(size*child + node2Index);
}