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

        Bitvector indexH = Bitvector_Utils::getEntry(iterD, index, miniTree.microTrees.cend(), Bitvector_Utils::BitvectorEncoding::HUFFMAN, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 2, 0, huffmanCodes});
        return std::find_if(lookupTable.begin(),lookupTable.end(), [&indexH](const LookupTableEntry& microTreeData){ return microTreeData.index == indexH;})->bp;
    }
    else {
        auto iterD = miniTree.microTrees.cbegin();
        return Bitvector_Utils::getEntry(iterD, index, miniTree.microTrees.cend(),
                                         Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,
                                         {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 2, 0});
    }
}

Bitvector HypersuccinctTree::getMicroFID(MiniTree& miniTree,uint32_t index) {
    auto iterD = miniTree.FIDs.cbegin();
    return Bitvector_Utils::getEntry(iterD, index, miniTree.FIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1, 0});
}

Bitvector HypersuccinctTree::getMicroTypeVector(MiniTree& miniTree , uint32_t index) {
    auto iterD = miniTree.typeVectors.cbegin();
    auto iterF = miniTree.FIDs.cbegin();
    return Bitvector_Utils::getEntry(iterD, index, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniTree.FIDs.cend(), 2, 0});
}

Bitvector HypersuccinctTree::getMicroDummys(MiniTree& miniTree, uint32_t index) {
    auto iter = microSize.cbegin();
    uint32_t size = pht::Bitvector_Utils::decodeNumber(iter, microSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*size+1))+1;
    auto iterD = miniTree.dummys.cbegin();
    return Bitvector_Utils::getEntry(iterD, index, miniTree.dummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 0, dummySize});
}

Bitvector HypersuccinctTree::getMiniDummy(uint32_t index) {
    auto iter = miniSize.cbegin();
    uint32_t size = pht::Bitvector_Utils::decodeNumber(iter, miniSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*size+1))+1;
    auto iterD = miniDummys.cbegin();
    return Bitvector_Utils::getEntry(iterD, index, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize});
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

bool HypersuccinctTree::miniTreeAncMatrixComparison(uint32_t ancTree, uint32_t treeIndex2) {
    auto iter = miniAncMatrix.cbegin();
    uint32_t size = sqrt(miniAncMatrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter, ancTree, miniAncMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 0, size});
    return segment.at(treeIndex2);
}

bool HypersuccinctTree::microTreeAncMatrixComparison(const MiniTree &miniTree, uint32_t ancTree, uint32_t treeIndex2) {
    auto iter = miniTree.ancMatrix.cbegin();
    uint32_t size = sqrt(miniTree.ancMatrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter, ancTree, miniTree.ancMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 0, size});
    return segment.at(treeIndex2);
}

bool HypersuccinctTree::lookupTableMatrixComparison(const LookupTableEntry& entry, uint32_t anc, uint32_t node2Index) {
    auto iter = entry.matrix.cbegin();
    uint32_t size = sqrt(entry.matrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter, anc, entry.matrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 0, size});
    return segment.at(node2Index);
}

Bitvector HypersuccinctTree::getFIDforMiniTree(uint32_t treeNum) {
    auto iterD = miniFIDs.cbegin();
    std::vector<Bitvector> fids;
    fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1, 0}));
    auto iterD2 = miniTypeVectors.cbegin();
    auto iterF = miniFIDs.cbegin();
    std::vector<Bitvector> tvs;
    tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));

    std::vector<Bitvector>dummys;
    auto iter = miniSize.cbegin();
    uint32_t size = pht::Bitvector_Utils::decodeNumber(iter, miniSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*size+1))+1;
    auto iterD3 = miniDummys.cbegin();
    dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));



    uint32_t topTrees = 0;
    uint32_t lowTrees = 0;

    uint32_t index = 0;
    while(index < fids.size()) {
        Bitvector fid = fids.at(index);
        uint32_t temp =  Bitvector_Utils::countOccurences(tvs.at(index).cbegin(), tvs.at(index).cend());
        if(temp == 0) {
            temp++;
        }
        topTrees += temp;
        if(treeNum < topTrees) {
            return fids.at(index);
        }
        lowTrees = Bitvector_Utils::countOccurences(tvs.at(index).cbegin(), tvs.at(index).cend(),true);
        if(iterD != miniFIDs.cend()) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
                tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));
                dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
            }
        }
        auto iterDummy = dummys.at(index).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(index).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
            tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));
            dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
        }
        index++;
    }
    return {};
}

Bitvector HypersuccinctTree::getFIDforMicroTree(MiniTree &miniTree, uint32_t treeNum) {
    auto iterD = miniTree.FIDs.cbegin();
    std::vector<Bitvector> fids;
    fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniTree.FIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1, 0}));
    auto iterD2 = miniTree.typeVectors.cbegin();
    auto iterF = miniTree.FIDs.cbegin();
    std::vector<Bitvector> tvs;
    tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniTree.FIDs.cend(), 2, 0}));

    std::vector<Bitvector>dummys;
    auto iter = microSize.cbegin();
    uint32_t size = pht::Bitvector_Utils::decodeNumber(iter, microSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*size+1))+1;
    auto iterD3 = miniTree.dummys.cbegin();
    dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniTree.dummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));



    uint32_t topTrees = 0;
    uint32_t lowTrees = 0;

    uint32_t index = 0;
    while(index < fids.size()) {
        Bitvector fid = fids.at(index);
        uint32_t temp =  Bitvector_Utils::countOccurences(tvs.at(index).cbegin(), tvs.at(index).cend());
        if(temp == 0) {
            temp++;
        }
        topTrees += temp;
        if(treeNum < topTrees) {
            return fids.at(index);
        }
        lowTrees = Bitvector_Utils::countOccurences(tvs.at(index).cbegin(), tvs.at(index).cend(),true);
        if(iterD != miniTree.FIDs.cend()) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniTree.FIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
                tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniTree.FIDs.cend(), 2, 0}));
                dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniTree.dummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
            }
        }
        auto iterDummy = dummys.at(index).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(index).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniTree.FIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
            tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniTree.FIDs.cend(), 2, 0}));
            dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniTree.dummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
        }
        index++;
    }
    return {};
}

Bitvector HypersuccinctTree::getFIDforMicroTree(uint32_t miniTree, uint32_t treeNum) {
    MiniTree miniTree1 = getMiniTree(miniTree);
    return getFIDforMicroTree(miniTree1, treeNum);
}

Bitvector HypersuccinctTree::getParentFIDMiniTree(uint32_t treeNum) {
    //TODO:
    auto iterD = miniFIDs.cbegin();
    std::vector<Bitvector> fids;
    fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1, 0}));
    auto iterD2 = miniTypeVectors.cbegin();
    auto iterF = miniFIDs.cbegin();
    std::vector<Bitvector> tvs;
    tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));

    std::vector<Bitvector>dummys;
    auto iter = miniSize.cbegin();
    uint32_t size = pht::Bitvector_Utils::decodeNumber(iter, miniSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*size+1))+1;
    auto iterD3 = miniDummys.cbegin();
    dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));



    uint32_t topTrees = 0;
    uint32_t lowTrees = 0;

    std::vector<Bitvector> tempFids;
    std::vector<Bitvector> tempTvs;

    uint32_t index = 0;
    while(index < fids.size()) {
        Bitvector fid = fids.at(index);
        uint32_t temp =  Bitvector_Utils::countOccurences(tvs.at(index).cbegin(), tvs.at(index).cend());
        if(temp == 0) {
            temp++;
        }
        uint32_t ancTemp = tvs.at(index).size();
        if(treeNum < topTrees + ancTemp) {
            return fids.at(index);
        }
        topTrees += temp;
        lowTrees = Bitvector_Utils::countOccurences(tvs.at(index).cbegin(), tvs.at(index).cend(),true);
        if(iterD != miniFIDs.cend()) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
                tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));
                dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
            }
        }
        auto iterDummy = dummys.at(index).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(index).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
            tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));
            dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
        }
        index++;
    }
    return {};
}

std::tuple< std::vector<uint32_t >,std::vector<uint32_t > > HypersuccinctTree::getTreesForFID(uint32_t index) {
    auto iterD = miniFIDs.cbegin();
    std::vector<Bitvector> fids;
    fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1, 0}));

    auto iterD2 = miniTypeVectors.cbegin();
    auto iterF = miniFIDs.cbegin();
    std::vector<Bitvector> tvs;
    tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));

    std::vector<Bitvector>dummys;
    auto iter = miniSize.cbegin();
    uint32_t miniSizeNum = pht::Bitvector_Utils::decodeNumber(iter, miniSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*miniSizeNum+1))+1;
    auto iterD3 = miniDummys.cbegin();
    dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));

    std::vector<uint32_t > topTree;
    topTree.push_back(0);
    uint32_t lowOffset = 1;
    uint32_t currentIndex = 0;

    while(currentIndex < fids.size()) {
        Bitvector fid = fids.at(currentIndex);
        uint32_t topTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend());
        uint32_t lowTrees = Bitvector_Utils::countOccurences(tvs.at(currentIndex).cbegin(), tvs.at(currentIndex).cend(),true);
        if(currentIndex == index) {
            std::vector<uint32_t > topTreeIndices;
            std::vector<uint32_t > lowTreeIndices;
            if(topTrees==0) {
                topTreeIndices.push_back(topTree.at(currentIndex));
            }
            for(int i = 0; i< topTrees; i++) {
                topTreeIndices.push_back(topTree.at(currentIndex)+i);
            }
            lowTreeIndices.reserve(lowTrees);
            for(int i = 0; i< lowTrees; i++) {
                lowTreeIndices.push_back(lowOffset + i);
            }
            return {topTreeIndices,lowTreeIndices};
        }
        for(int i=1; i<= topTrees; i++) {
            if(topTree.size()<=currentIndex+i) {
                topTree.push_back(topTree.at(currentIndex) + topTrees);
            }
        }
        if(topTrees==0) {
            topTree.push_back(topTree.at(currentIndex) + 1);
        }
        lowOffset += lowTrees;


        if(iterD != miniFIDs.cend()) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
                tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));
                dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
            }
        }
        auto iterDummy = dummys.at(currentIndex).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
            tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));
            dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
        }
        currentIndex++;
    }
    return {{},{}};
}

std::pair<uint32_t ,uint32_t > HypersuccinctTree::TreeToFIDIndexConversion(uint32_t miniTree) {
    auto iterD = miniFIDs.cbegin();
    std::vector<Bitvector> fids;
    fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1, 0}));

    auto iterD2 = miniTypeVectors.cbegin();
    auto iterF = miniFIDs.cbegin();
    std::vector<Bitvector> tvs;
    tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));

    std::vector<Bitvector>dummys;
    auto iter = miniSize.cbegin();
    uint32_t miniSizeNum = pht::Bitvector_Utils::decodeNumber(iter, miniSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t dummySize = floor(log2(2*miniSizeNum+1))+1;
    auto iterD3 = miniDummys.cbegin();
    dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));

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
        for(int i=0; i<childFIDs.size(); i++) {
            childIndex += childFIDs.at(i);
            if(currentIndex <= childIndex && miniTree < topOffset + topTrees) {
                lowIndex = i;
                break;
            }
        }

        childFIDs.push_back(lowTrees);

        //TopTree Index Conversion
        if(miniTree < topOffset + topTrees) {
            topIndex = currentIndex;
            return {topIndex,lowIndex};
        }
        topOffset += topTrees;

        //Getting new FIDs
        if(iterD != miniFIDs.cend()) {
            for (uint32_t i = 0; i < lowTrees; i++) {
                fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
                tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));
                dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
            }
        }
        auto iterDummy = dummys.at(currentIndex).cbegin();
        if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,1, 0}));
            tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniFIDs.cend(), 2, 0}));
            dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator,0, dummySize}));
        }
        currentIndex++;
    }
    return {-1,-1};
}