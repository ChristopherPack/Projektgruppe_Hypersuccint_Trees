//
// Created by Christopher Pack on 23.04.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>


using namespace pht;

Bitvector HypersuccinctTree::getMicroTree(MiniTree& miniTree,uint32_t index) {
    if(huffmanFlag) {
        /*auto iterD = miniTree.microTrees.cbegin();
        std::set<Bitvector, Bitvector_Utils::HuffmanComparator> huffmanCodes;
        for(LookupTableEntry& microTreeData : lookupTable) {
            huffmanCodes.insert(microTreeData.index);
        }

        Bitvector indexH = Bitvector_Utils::getEntry(iterD, index, miniTree.microTrees.cend(), Bitvector_Utils::BitvectorEncoding::HUFFMAN, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 2, 0, huffmanCodes});
        return std::find_if(lookupTable.begin(),lookupTable.end(), [&indexH](const LookupTableEntry& microTreeData){ return microTreeData.index == indexH;})->bp;*/
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
    auto iter = entry.ancestorMatrix.cbegin();
    uint32_t size = sqrt(entry.ancestorMatrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter, anc, entry.ancestorMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 0, size});
    return segment.at(node2Index);
}

bool HypersuccinctTree::lookupTableChildMatrixComparison(const LookupTableEntry& entry, uint32_t child, uint32_t node2Index) {
    auto iter = entry.childMatrix.cbegin();
    uint32_t size = sqrt(entry.childMatrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter, child, entry.ancestorMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 0, size});
    return segment.at(node2Index);
}

Bitvector HypersuccinctTree::getFIDforMiniTree(uint32_t treeNum) {
    auto iterD = miniFIDs.cbegin();
    std::vector<Bitvector> fids = {*iterD};
    iterD++;
    auto iterD2 = miniTypeVectors.cbegin();
    std::vector<Bitvector> tvs = {*iterD2};
    iterD2++;
    auto iterD3 = miniDummys.cbegin();
    std::vector<Bitvector> dummys = {*iterD3};
    iterD3++;

    uint32_t topOffset = 0;
    uint32_t lowOffset = 0;
    uint32_t currentIndex = 0;

    while(currentIndex < fids.size()) {
        Bitvector fid = fids.at(currentIndex);
        uint32_t topTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend());
        uint32_t lowTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend(),true);

        if(topTrees == 0) {
            topTrees = 1;
        }
        if(currentIndex==0) {
            lowOffset = topTrees;
        }

        topOffset += topTrees;

        lowOffset += lowTrees;

        if(treeNum < topOffset) {
            return fids.at(currentIndex);
        }

        if(miniFIDs.cend() - iterD > 0) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(*iterD);
                iterD++;
                tvs.push_back(*iterD2);
                iterD2++;
                dummys.push_back(*iterD3);
                iterD3++;
            }
        }
        auto iterDummy = dummys.at(currentIndex).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(*iterD);
            iterD++;
            tvs.push_back(*iterD2);
            iterD2++;
            dummys.push_back(*iterD3);
            iterD3++;
        }
        currentIndex++;
    }
    return {};
}

Bitvector HypersuccinctTree::getFIDforMicroTree(MiniTree &miniTree, uint32_t treeNum) {
    auto iterD = miniTree.FIDs.cbegin();
    std::vector<Bitvector> fids = {*iterD};
    iterD++;
    auto iterD2 = miniTree.typeVectors.cbegin();
    std::vector<Bitvector> tvs = {*iterD2};
    iterD2++;
    auto iterD3 = miniTree.dummys.cbegin();
    std::vector<Bitvector> dummys = {*iterD3};
    iterD3++;


    uint32_t topOffset = 0;
    uint32_t lowOffset = 0;
    uint32_t currentIndex = 0;

    while(currentIndex < fids.size()) {
        Bitvector fid = fids.at(currentIndex);
        uint32_t topTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend());
        uint32_t lowTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend(),true);

        if(topTrees == 0) {
            topTrees = 1;
        }
        if(currentIndex==0) {
            lowOffset = topTrees;
        }
        topOffset += topTrees;

        lowOffset += lowTrees;


        if(treeNum < topOffset) {
            return fids.at(currentIndex);
        }

        if(miniTree.FIDs.cend() - iterD > 0) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(*iterD);
                iterD++;
                tvs.push_back(*iterD2);
                iterD2++;
                dummys.push_back(*iterD3);
                iterD3++;
            }
        }
        auto iterDummy = dummys.at(currentIndex).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(*iterD);
            iterD++;
            tvs.push_back(*iterD2);
            iterD2++;
            dummys.push_back(*iterD3);
            iterD3++;
        }
        currentIndex++;
    }
    return {};
}

Bitvector HypersuccinctTree::getFIDforMicroTree(uint32_t miniTree, uint32_t treeNum) {
    MiniTree miniTree1 = getMiniTree(miniTree);
    return getFIDforMicroTree(miniTree1, treeNum);
}

std::pair< std::vector<uint32_t >,std::vector<uint32_t > > HypersuccinctTree::getTreesForFID(uint32_t index) {
    auto iterD = miniFIDs.cbegin();
    std::vector<Bitvector> fids = {*iterD};
    iterD++;
    auto iterD2 = miniTypeVectors.cbegin();
    std::vector<Bitvector> tvs = {*iterD2};
    iterD2++;
    auto iterD3 = miniDummys.cbegin();
    std::vector<Bitvector> dummys = {*iterD3};
    iterD3++;

    std::vector<uint32_t > topTree;
    topTree.push_back(0);
    uint32_t topOffset = 0;
    uint32_t lowOffset = 0;
    uint32_t currentIndex = 0;

    while(currentIndex < fids.size()) {
        Bitvector fid = fids.at(currentIndex);
        uint32_t topTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend());
        uint32_t lowTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend(),true);

        if(topTrees == 0) {
            topTrees = 1;
        }
        if(currentIndex==0) {
            lowOffset = topTrees;
        }

        if(currentIndex == index) {
            std::vector<uint32_t > topTreeIndices;
            std::vector<uint32_t > lowTreeIndices;
            topTreeIndices.reserve(topTrees);
            for(int i = 0; i< topTrees; i++) {
                topTreeIndices.push_back(topTree.at(topOffset)+i);
            }
            lowTreeIndices.reserve(lowTrees);
            for(int i = 0; i< lowTrees; i++) {
                lowTreeIndices.push_back(lowOffset + i);
            }
            return {topTreeIndices,lowTreeIndices};
        }

        for(int i=1; i<= topTrees; i++) {
            if(topTree.size()<=topOffset+i) {
                topTree.push_back(topTree.at(topOffset) + topTrees);
            }
        }
        topOffset += topTrees;

        lowOffset += lowTrees;


        if(miniFIDs.cend() - iterD > 0) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(*iterD);
                iterD++;
                tvs.push_back(*iterD2);
                iterD2++;
                dummys.push_back(*iterD3);
                iterD3++;
            }
        }
        auto iterDummy = dummys.at(currentIndex).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(*iterD);
            iterD++;
            tvs.push_back(*iterD2);
            iterD2++;
            dummys.push_back(*iterD3);
            iterD3++;
        }
        currentIndex++;
    }
    return {{},{}};
}

std::pair< std::vector<uint32_t >,std::vector<uint32_t > > HypersuccinctTree::getTreesForMicroFID(MiniTree &miniTree, uint32_t index) {
    auto iterD = miniTree.FIDs.cbegin();
    std::vector<Bitvector> fids = {*iterD};
    iterD++;
    auto iterD2 = miniTree.typeVectors.cbegin();
    std::vector<Bitvector> tvs = {*iterD2};
    iterD2++;
    auto iterD3 = miniTree.dummys.cbegin();
    std::vector<Bitvector> dummys = {*iterD3};
    iterD3++;

    std::vector<uint32_t > topTree;
    topTree.push_back(0);
    uint32_t topOffset = 0;
    uint32_t lowOffset = 0;
    uint32_t currentIndex = 0;

    while(currentIndex < fids.size()) {
        Bitvector fid = fids.at(currentIndex);
        uint32_t topTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend());
        uint32_t lowTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend(),true);

        if(topTrees == 0) {
            topTrees = 1;
        }
        if(currentIndex==0) {
            lowOffset = topTrees;
        }

        if(currentIndex == index) {
            std::vector<uint32_t > topTreeIndices;
            std::vector<uint32_t > lowTreeIndices;
            topTreeIndices.reserve(topTrees);
            for(int i = 0; i< topTrees; i++) {
                topTreeIndices.push_back(topTree.at(topOffset)+i);
            }
            lowTreeIndices.reserve(lowTrees);
            for(int i = 0; i< lowTrees; i++) {
                lowTreeIndices.push_back(lowOffset + i);
            }
            return {topTreeIndices,lowTreeIndices};
        }

        for(int i=1; i<= topTrees; i++) {
            if(topTree.size()<=topOffset+i) {
                topTree.push_back(topTree.at(topOffset) + topTrees);
            }
        }
        topOffset += topTrees;

        lowOffset += lowTrees;


        if(miniTree.FIDs.cend() - iterD > 0) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(*iterD);
                iterD++;
                tvs.push_back(*iterD2);
                iterD2++;
                dummys.push_back(*iterD3);
                iterD3++;
            }
        }
        auto iterDummy = dummys.at(currentIndex).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(*iterD);
            iterD++;
            tvs.push_back(*iterD2);
            iterD2++;
            dummys.push_back(*iterD3);
            iterD3++;
        }
        currentIndex++;
    }
    return {{},{}};
}

std::pair<uint32_t ,uint32_t > HypersuccinctTree::convertTreeToFIDIndex(uint32_t miniTree) {
    auto iterD = miniFIDs.cbegin();
    std::vector<Bitvector> fids = {*iterD};
    iterD++;
    auto iterD2 = miniTypeVectors.cbegin();
    std::vector<Bitvector> tvs = {*iterD2};
    iterD2++;
    auto iterD3 = miniDummys.cbegin();
    std::vector<Bitvector> dummys = {*iterD3};
    iterD3++;

    uint32_t topOffset = 0;
    std::vector<uint32_t > childFIDs;
    uint32_t currentIndex = 0;
    uint32_t topIndex = -1;
    uint32_t lowIndex = -1;

    while(currentIndex < fids.size()) {
        Bitvector fid = fids.at(currentIndex);
        uint32_t topTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend());
        uint32_t lowTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend(),true);
        //This is obvious: its a single node miniTree
        if(topTrees == 0) {
            topTrees = 1;
        }

        //LowTree Index Conversion
        uint32_t childIndex = 0;
        bool found = false;
        for(int i=0; i<childFIDs.size(); i++) {
            childIndex += childFIDs.at(i);
            if(currentIndex <= childIndex) {
                found = true;
                if(miniTree < topOffset + topTrees) {
                    lowIndex = i;
                }
                break;
            }
        }
        if(currentIndex > 0 && !found) {
            childFIDs.push_back(topTrees + lowTrees);
        }
        else {
            childFIDs.push_back(lowTrees);
        }

        //TopTree Index Conversion
        if(miniTree < topOffset + topTrees) {
            topIndex = currentIndex;
            return {topIndex,lowIndex};
        }
        topOffset += topTrees;

        //Getting new FIDs
        if(miniFIDs.cend() - iterD > 0) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(*iterD);
                iterD++;
                tvs.push_back(*iterD2);
                iterD2++;
                dummys.push_back(*iterD3);
                iterD3++;
            }
        }
        auto iterDummy = dummys.at(currentIndex).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(*iterD);
            iterD++;
            tvs.push_back(*iterD2);
            iterD2++;
            dummys.push_back(*iterD3);
            iterD3++;
        }
        currentIndex++;
    }
    return {-1,-1};
}

std::pair<uint32_t ,uint32_t > HypersuccinctTree::convertMicroTreeToFIDIndex(MiniTree &miniTree, uint32_t microTree) {
    auto iterD = miniTree.FIDs.cbegin();
    std::vector<Bitvector> fids = {*iterD};
    iterD++;
    auto iterD2 = miniTree.typeVectors.cbegin();
    std::vector<Bitvector> tvs = {*iterD2};
    iterD2++;
    auto iterD3 = miniTree.dummys.cbegin();
    std::vector<Bitvector> dummys = {*iterD3};
    iterD3++;

    uint32_t topOffset = 0;
    std::vector<uint32_t > childFIDs;
    uint32_t currentIndex = 0;
    uint32_t topIndex = -1;
    uint32_t lowIndex = -1;

    while(currentIndex < fids.size()) {
        Bitvector fid = fids.at(currentIndex);
        uint32_t topTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend());
        uint32_t lowTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend(),true);
        //This is obvious: its a single node miniTree
        if(topTrees == 0) {
            topTrees = 1;
        }

        //LowTree Index Conversion
        uint32_t childIndex = 0;
        bool found = false;
        for(int i=0; i<childFIDs.size(); i++) {
            childIndex += childFIDs.at(i);
            if(currentIndex <= childIndex) {
                found = true;
                if(microTree < topOffset + topTrees) {
                    lowIndex = i;
                }
                break;
            }
        }
        if(currentIndex > 0 && !found) {
            childFIDs.push_back(topTrees + lowTrees);
        }
        else {
            childFIDs.push_back(lowTrees);
        }

        //TopTree Index Conversion
        if(microTree < topOffset + topTrees) {
            topIndex = currentIndex;
            return {topIndex,lowIndex};
        }
        topOffset += topTrees;

        //Getting new FIDs
        if(miniTree.FIDs.cend() - iterD > 0) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(*iterD);
                iterD++;
                tvs.push_back(*iterD2);
                iterD2++;
                dummys.push_back(*iterD3);
                iterD3++;
            }
        }
        auto iterDummy = dummys.at(currentIndex).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(*iterD);
            iterD++;
            tvs.push_back(*iterD2);
            iterD2++;
            dummys.push_back(*iterD3);
            iterD3++;
        }
        currentIndex++;
    }
    return {-1,-1};
}