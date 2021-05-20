//
// Created by Christopher Pack on 23.04.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>


using namespace pht;

Bitvector HypersuccinctTree::getMicroTree(MiniTree& miniTree,uint32_t index) {
    if(huffmanFlag) {
        auto iterD = miniTree.microTrees.cbegin();
        std::set<Bitvector, Bitvector_Utils::HuffmanComparator> huffmanCodes;
        for(LookupTableEntry& microTreeData : lookupTable) {
            huffmanCodes.insert(microTreeData.index);
        }

        Bitvector indexH = Bitvector_Utils::getEntry(iterD, index, miniTree.microTrees.cend(), Bitvector_Utils::BitvectorEncoding::HUFFMAN, {2, 0, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, huffmanCodes});
        return std::find_if(lookupTable.begin(),lookupTable.end(), [&indexH](const LookupTableEntry& microTreeData){ return microTreeData.index == indexH;})->bp;
    }
    else {
        auto iterD = miniTree.microTrees.cbegin();
        return Bitvector_Utils::getEntry(iterD, index, miniTree.microTrees.cend(),
                                         Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,
                                         {2, 0, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator});
    }
}

Bitvector HypersuccinctTree::getMicroFID(MiniTree& miniTree,uint32_t index) {
    auto iterD = miniTree.FIDs.cbegin();
    return Bitvector_Utils::getEntry(iterD, index, miniTree.FIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {1, 0, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator});
}

Bitvector HypersuccinctTree::getMicroTypeVector(MiniTree& miniTree , uint32_t index) {
    auto iterD = miniTree.typeVectors.cbegin();
    auto iterF = miniTree.FIDs.cbegin();
    return Bitvector_Utils::getEntry(iterD, index, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, {2, 0, iterF, miniTree.FIDs.cend()});
}

Bitvector HypersuccinctTree::getMicroDummys(MiniTree& miniTree, uint32_t index) {
    auto iter = microSize.cbegin();
    uint32_t size = pht::Bitvector_Utils::decodeNumber(iter, microSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*size+1))+1;
    auto iterD = miniTree.dummys.cbegin();
    return Bitvector_Utils::getEntry(iterD, index, miniTree.dummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {0, dummySize, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator});
}

Bitvector HypersuccinctTree::getMiniDummy(uint32_t index) {
    auto iter = miniSize.cbegin();
    uint32_t size = pht::Bitvector_Utils::decodeNumber(iter, miniSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*size+1))+1;
    auto iterD = miniDummys.cbegin();
    return Bitvector_Utils::getEntry(iterD, index, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {0, dummySize, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator});
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

bool HypersuccinctTree::lookupTableMatrixComparison(const LookupTableEntry& entry, uint32_t node1Index, uint32_t node2Index) {
    auto iter = entry.matrix.cbegin();
    uint32_t size = sqrt(entry.matrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter, node1Index, entry.matrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {0, size, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator});
    return segment.at(node2Index);
}