#include <iostream>
#include <fstream>
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"


using namespace pht;
using namespace std;

void HypersuccinctTreeOutput::printTree(HypersuccinctTree &tree) {
    PHT_LOGGER_INFO("HST_OUT") << "Hypersuccinct Tree:" << "\n" << pht::Logger::endl();
    PHT_LOGGER_INFO("HST_OUT") << "IsHuffman:   " << pht::Logger::endl();
    PHT_LOGGER_INFO("HST_OUT") << tree.isHuffman() << "\n" << pht::Logger::endl();
    PHT_LOGGER_INFO("HST_OUT") << "TreeSize:  " << pht::Logger::endl();
    printBitvector(tree.getSize());
    PHT_LOGGER_INFO("HST_OUT") << "MiniSize:  " << pht::Logger::endl();
    printBitvector(tree.getMiniSize());
    PHT_LOGGER_INFO("HST_OUT") << "MicroSize:  " << pht::Logger::endl();
    printBitvector(tree.getMicroSize());
    PHT_LOGGER_INFO("HST_OUT") << "\n" << pht::Logger::endl();

    PHT_LOGGER_INFO("HST_OUT") << "Amount of MiniTrees: " << tree.getMiniTrees().size() << "\n" << pht::Logger::endl();
    PHT_LOGGER_INFO("HST_OUT") << "MiniFIDs:  " << pht::Logger::endl();
    printBitvector(tree.getMiniFIDs());
    PHT_LOGGER_INFO("HST_OUT") << "FID Top Trees:  " << pht::Logger::endl();
    printBitvector(tree.getMiniFIDs());
    PHT_LOGGER_INFO("HST_OUT") << "FID Low Trees:  " << pht::Logger::endl();
    printBitvector(tree.getMiniFIDs());
    PHT_LOGGER_INFO("HST_OUT") << "MiniTypeVectors:  " << pht::Logger::endl();
    printBitvector(tree.getMiniTypeVectors());
    PHT_LOGGER_INFO("HST_OUT") << "MiniDummys:  " << pht::Logger::endl();
    printBitvector(tree.getMiniDummys());

    pht::MiniTree miniTree = tree.getMiniTree(0);
    for(int index = 0 ; index < tree.getMiniTrees().size(); index++) {
        PHT_LOGGER_INFO("HST_OUT") << "MicroTreesinMiniTree" << index << ":  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microTrees);
        PHT_LOGGER_INFO("HST_OUT") << "MicroTreeFIDs:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).FIDs);
        PHT_LOGGER_INFO("HST_OUT") << "MicroTreeTypeVectors:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).typeVectors);
        PHT_LOGGER_INFO("HST_OUT") << "MicroTreeDummys:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).dummys);

        PHT_LOGGER_INFO("HST_OUT") << "Mini Top FID Index:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniTopFIDIndex);
        PHT_LOGGER_INFO("HST_OUT") << "Mini Low FID Index:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniLowFIDIndex);
        PHT_LOGGER_INFO("HST_OUT") << "Micro Top FID Indices:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microTopFIDIndices);
        PHT_LOGGER_INFO("HST_OUT") << "Micro Low FID Indices:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microLowFIDIndices);
        PHT_LOGGER_INFO("HST_OUT") << "Micro FID Top Trees:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microFIDTopTrees);
        PHT_LOGGER_INFO("HST_OUT") << "Micro FID Low Trees:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microFIDLowTrees);

        PHT_LOGGER_INFO("HST_OUT") << "RootAncestors:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).rootAncestors);
        PHT_LOGGER_INFO("HST_OUT") << "DummyAncestors:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).dummyAncestors);
        PHT_LOGGER_INFO("HST_OUT") << "MiniDummyTree:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniDummyTree);
        PHT_LOGGER_INFO("HST_OUT") << "MiniDummyIndex:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniDummyIndex);
        PHT_LOGGER_INFO("HST_OUT") << "MiniDummyPointer:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniDummyPointer);
        PHT_LOGGER_INFO("HST_OUT") << "MicroDummyPointers:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microDummyPointers);

        PHT_LOGGER_INFO("HST_OUT") << "Mini Child Rankd:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniChildRank);
        PHT_LOGGER_INFO("HST_OUT") << "Micro Child Ranks:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microChildRanks);
        PHT_LOGGER_INFO("HST_OUT") << "Micro extended Child Ranks:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microExtendedChildRanks);

        PHT_LOGGER_INFO("HST_OUT") << "Mini Parent Pointers:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniParent);
        PHT_LOGGER_INFO("HST_OUT") << "Micro Parent Pointers:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microParents);

        PHT_LOGGER_INFO("HST_OUT") << "Subtree Size at MiniTree Root:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).subTree);
        PHT_LOGGER_INFO("HST_OUT") << "SubTreeSize at MicroTree Roots:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microSubTrees);
        PHT_LOGGER_INFO("HST_OUT") << "Depth at MiniTree Root:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniDepth);
        PHT_LOGGER_INFO("HST_OUT") << "Height at MiniTree Root:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniHeight);
        PHT_LOGGER_INFO("HST_OUT") << "Depth at MiniTree Dummy:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniDummyDepth);
        PHT_LOGGER_INFO("HST_OUT") << "Height at MiniTree Dummy:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniDummyHeight);
        PHT_LOGGER_INFO("HST_OUT") << "Depth at MicroTree Roots:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).rootDepths);
        PHT_LOGGER_INFO("HST_OUT") << "Height at MicroTree Roots:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).rootHeights);
        PHT_LOGGER_INFO("HST_OUT") << "Leaf Size at MiniTree Root:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniLeaves);
        PHT_LOGGER_INFO("HST_OUT") << "Leaf Size at MicroTree Roots:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microLeaves);
        PHT_LOGGER_INFO("HST_OUT") << "Leftmost MiniTree:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniTreeLeftmostLeafPointer);
        PHT_LOGGER_INFO("HST_OUT") << "Rightmost MiniTree:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniTreeRightmostLeafPointer);
        PHT_LOGGER_INFO("HST_OUT") << "Leftmost MicroTree within MiniTree:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microTreeLeftmostLeafPointers);
        PHT_LOGGER_INFO("HST_OUT") << "Rightmost MicroTree within MiniTree:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microTreeRightmostLeafPointers);
        PHT_LOGGER_INFO("HST_OUT") << "LeafRank at MiniTree Root:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniRootLeafRank);
        PHT_LOGGER_INFO("HST_OUT") << "LeafRank at MiniTree Dummy:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).miniDummyLeafRank);
        PHT_LOGGER_INFO("HST_OUT") << "LeafRank at MicroTree Roots:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microRootLeafRanks);
        PHT_LOGGER_INFO("HST_OUT") << "Extended Leaf Rank of MicroTree Roots:  " << pht::Logger::endl();
        printBitvector(tree.getMiniTree(index).microExtendedLeafRanks);
        PHT_LOGGER_INFO("HST_OUT") << "\n" << pht::Logger::endl();
    }
    for(uint32_t index = 0; index < tree.getLookupTable().size(); index++) {
        PHT_LOGGER_INFO("HST_OUT") << "LookupTableIndex:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).index);
        PHT_LOGGER_INFO("HST_OUT")<< "BP for Huffman:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).bp);
        PHT_LOGGER_INFO("HST_OUT") << "AncestorMap:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).ancestorMatrix);
        PHT_LOGGER_INFO("HST_OUT") << "ChildMap:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).childMatrix);
        PHT_LOGGER_INFO("HST_OUT") << "ChildRanks:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).childRanks);
        PHT_LOGGER_INFO("HST_OUT") << "Parent Pointers:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).parentPointers);
        PHT_LOGGER_INFO("HST_OUT") << "Degrees:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).degree);
        PHT_LOGGER_INFO("HST_OUT") << "Subtrees:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).subTrees);
        PHT_LOGGER_INFO("HST_OUT") << "Depths:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).nodeDepths);
        PHT_LOGGER_INFO("HST_OUT") << "Heights:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).nodeHeights);
        PHT_LOGGER_INFO("HST_OUT") << "Leaf Sizes:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).leaves);
        PHT_LOGGER_INFO("HST_OUT") << "Leftmost Leaves:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).leftmost_leaf);
        PHT_LOGGER_INFO("HST_OUT") << "Rightmost Leaves:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).rightmost_leaf);
        PHT_LOGGER_INFO("HST_OUT") << "Leaf Ranks:   " << pht::Logger::endl();
        printBitvector(tree.getLookupTableEntry(index).leafRank);
    }
}

void HypersuccinctTreeOutput::printBitvector(const vector<bool>& bitvector) {
    std::stringstream ss;
    for(bool bit: bitvector) {
        ss << bit;
    }
    PHT_LOGGER_INFO("HST_OUT") << ss.str() << "\n" << pht::Logger::endl();
}

void HypersuccinctTreeOutput::printBitvector(const vector<vector<bool>> &bitvector) {
    std::stringstream ss;
    for (const std::vector<bool>& part : bitvector) {
        for(bool bit: part) {
            ss << bit;
        }
        ss << "  ";
    }
    PHT_LOGGER_INFO("HST_OUT") << ss.str() << "\n" << pht::Logger::endl();
}

void HypersuccinctTreeOutput::printBitvector(const vector<vector<vector<bool>>> &bitvector) {
    std::stringstream ss;
    for(const std::vector<vector<bool>> &first: bitvector) {
        for (const std::vector<bool> &part: first) {
            for (bool bit: part) {
                ss << bit;
            }
            ss << "  ";
        }
        ss << ";  ";
    }
    PHT_LOGGER_INFO("HST_OUT") << ss.str() << "\n" << pht::Logger::endl();
}


void HypersuccinctTreeOutput::writeToFile(HypersuccinctTree &tree,const std::string& filename) {
    //encode mit Elias Gamma
    std::ofstream file;
    file.open(filename, std::ofstream::binary);
    Bitvector fileBitvector;
    fileBitvector.push_back(tree.isHuffman());
    BitvectorUtils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()),BitvectorUtils::decodeNumber(tree.getSize(),BitvectorUtils::NumberEncoding::BINARY),BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    BitvectorUtils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()),BitvectorUtils::decodeNumber(tree.getMiniSize(),BitvectorUtils::NumberEncoding::BINARY),BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    BitvectorUtils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()),BitvectorUtils::decodeNumber(tree.getMicroSize(),BitvectorUtils::NumberEncoding::BINARY),BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    BitvectorUtils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()), static_cast<uint32_t>(tree.getMiniTrees().size()), BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    BitvectorUtils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()), static_cast<uint32_t>(tree.getLookupTable().size()), BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
    Bitvector emptySet = {false};

    createFileBitvector(tree.getMiniFIDs(), fileBitvector);
    createFileBitvector(tree.getFIDTopTrees(), fileBitvector);
    createFileBitvector(tree.getFIDLowTrees(), fileBitvector);
    createFileBitvector(tree.getMiniTypeVectors(), fileBitvector);
    createFileBitvector(tree.getMiniDummys(), fileBitvector);
    for(const MiniTree& miniTree : tree.getMiniTrees()) {
        createFileBitvector(miniTree.FIDs, fileBitvector);
        createFileBitvector(miniTree.typeVectors, fileBitvector);
        createFileBitvector(miniTree.dummys, fileBitvector);
        createFileBitvector(miniTree.microTrees, fileBitvector);

        createFileBitvector(miniTree.miniTopFIDIndex, fileBitvector);
        createFileBitvector(miniTree.miniLowFIDIndex, fileBitvector);
        createFileBitvector(miniTree.microTopFIDIndices, fileBitvector);
        createFileBitvector(miniTree.microLowFIDIndices, fileBitvector);
        createFileBitvector(miniTree.microFIDTopTrees, fileBitvector);
        createFileBitvector(miniTree.microFIDLowTrees, fileBitvector);

        createFileBitvector(miniTree.rootAncestors, fileBitvector);
        createFileBitvector(miniTree.dummyAncestors, fileBitvector);
        createFileBitvector(miniTree.miniDummyTree, fileBitvector);
        createFileBitvector(miniTree.miniDummyIndex, fileBitvector);
        createFileBitvector(miniTree.miniDummyPointer, fileBitvector);
        createFileBitvector(miniTree.microDummyPointers, fileBitvector);

        createFileBitvector(miniTree.miniChildRank, fileBitvector);
        createFileBitvector(miniTree.microChildRanks, fileBitvector);
        createFileBitvector(miniTree.microExtendedChildRanks, fileBitvector);

        createFileBitvector(miniTree.miniParent, fileBitvector);
        createFileBitvector(miniTree.microParents, fileBitvector);

        createFileBitvector(miniTree.subTree, fileBitvector);
        createFileBitvector(miniTree.microSubTrees, fileBitvector);
        createFileBitvector(miniTree.miniDepth, fileBitvector);
        createFileBitvector(miniTree.miniHeight, fileBitvector);
        createFileBitvector(miniTree.miniDummyDepth, fileBitvector);
        createFileBitvector(miniTree.miniDummyHeight, fileBitvector);
        createFileBitvector(miniTree.rootDepths, fileBitvector);
        createFileBitvector(miniTree.rootHeights, fileBitvector);
        createFileBitvector(miniTree.miniLeaves, fileBitvector);
        createFileBitvector(miniTree.microLeaves, fileBitvector);
        createFileBitvector(miniTree.miniTreeLeftmostLeafPointer, fileBitvector);
        createFileBitvector(miniTree.miniTreeRightmostLeafPointer, fileBitvector);
        createFileBitvector(miniTree.microTreeLeftmostLeafPointers, fileBitvector);
        createFileBitvector(miniTree.microTreeRightmostLeafPointers, fileBitvector);
        createFileBitvector(miniTree.miniRootLeafRank, fileBitvector);
        createFileBitvector(miniTree.miniDummyLeafRank, fileBitvector);
        createFileBitvector(miniTree.microRootLeafRanks, fileBitvector);
        createFileBitvector(miniTree.microExtendedLeafRanks, fileBitvector);
    }
    for(const LookupTableEntry& microTreeData : tree.getLookupTable()) {
        createFileBitvector(microTreeData.index, fileBitvector);
        createFileBitvector(microTreeData.bp, fileBitvector);
        createFileBitvector(microTreeData.ancestorMatrix, fileBitvector);
        createFileBitvector(microTreeData.childMatrix, fileBitvector);
        createFileBitvector(microTreeData.childRanks, fileBitvector);
        createFileBitvector(microTreeData.parentPointers, fileBitvector);
        createFileBitvector(microTreeData.degree, fileBitvector);
        createFileBitvector(microTreeData.subTrees, fileBitvector);
        createFileBitvector(microTreeData.nodeDepths, fileBitvector);
        createFileBitvector(microTreeData.nodeHeights, fileBitvector);
        createFileBitvector(microTreeData.leaves, fileBitvector);
        createFileBitvector(microTreeData.leftmost_leaf, fileBitvector);
        createFileBitvector(microTreeData.rightmost_leaf, fileBitvector);
        createFileBitvector(microTreeData.leafRank, fileBitvector);
    }
    //Padding - see CreateFromFile
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    writeBitvectorToFile(file,fileBitvector);
    file.close();
}

void HypersuccinctTreeOutput::writeHuffmanToFile(HypersuccinctTree &tree, const std::string& filename, bool allData) {
    std::ofstream file;
    file.open(filename, std::ofstream::binary);
    Bitvector fileBitvector;
    for(const MiniTree& miniTree : tree.getMiniTrees()) {
        createFileBitvector(miniTree.microTrees,fileBitvector);
    }
    if(allData) {
        for(const LookupTableEntry& microTreeData : tree.getLookupTable()) {
            createFileBitvector(microTreeData.index,fileBitvector);
            createFileBitvector(microTreeData.bp, fileBitvector);
        }
    }
    //Padding - see CreateFromFile
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    fileBitvector.push_back(true);
    writeBitvectorToFile(file,fileBitvector);
    file.close();
}

HypersuccinctTree HypersuccinctTreeOutput::readFromFile(const string& path) {
    std::ifstream file;
    file.open("tree.txt", std::ifstream::binary);
    Bitvector fileBitvector = readBitvectorFromFile(file);
    file.close();
    //funktion in Factory:
    return HypersuccinctTreeFactory::createFromBitvector(fileBitvector);
}

void HypersuccinctTreeOutput::createFileBitvector(const Bitvector& bitvector, Bitvector& target) {
    if(bitvector.empty()) {
        target.push_back(true);
        target.push_back(false);
    } else {
        BitvectorUtils::encodeNumber(std::inserter(target, target.end()),
                                      static_cast<uint32_t>(bitvector.size()), BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
        ListUtils::combine(target, bitvector);
    }
}

void HypersuccinctTreeOutput::createFileBitvector(const std::vector<Bitvector>& bitvector, Bitvector &target) {
    if(bitvector.empty()) {
        target.push_back(true);
        target.push_back(false);
    } else {
        Bitvector partFileBit;
        for(const Bitvector& part : bitvector) {
            createFileBitvector(part,partFileBit);
        }
        BitvectorUtils::encodeNumber(std::inserter(target,target.end()), static_cast<uint32_t>(bitvector.size()), BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
        ListUtils::combine(target,partFileBit);
    }
}

void HypersuccinctTreeOutput::createFileBitvector(const std::vector<std::vector<Bitvector>>& bitvector, Bitvector &target) {
    if(bitvector.empty()) {
        target.push_back(true);
        target.push_back(false);
    } else {
        Bitvector partFileBit;
        for(const std::vector<Bitvector>& part : bitvector) {
            createFileBitvector(part,partFileBit);
        }
        BitvectorUtils::encodeNumber(std::inserter(target,target.end()), static_cast<uint32_t>(bitvector.size()), BitvectorUtils::NumberEncoding::ELIAS_GAMMA);
        ListUtils::combine(target,partFileBit);
    }
}

void HypersuccinctTreeOutput::writeBitvectorToFile(std::ofstream &file, Bitvector& bitvector) {
    uint32_t bytes = 0;
    Bitvector tmp;
    for(uint32_t i = 0; i < bitvector.size()/8; i++) {
        tmp.clear();
        for(uint32_t j = 0; j < 8; j++) {
            tmp.push_back(bitvector.at(i*8+j));
        }
        auto iter = tmp.cbegin();
        uint32_t num = pht::BitvectorUtils::decodeNumber(iter, tmp.cend(),BitvectorUtils::NumberEncoding::BINARY);
        file.write(reinterpret_cast<char*>(&num), 1);
        bytes++;
    }
    tmp.clear();
    for(uint32_t i = 0; i < bitvector.size()%8; i++) {
        tmp.push_back(bitvector.at(bytes+i));
    }
    for(uint32_t i = 0; i < 8-(bitvector.size()%8); i++) {
        tmp.push_back(false);
    }
    auto iter = tmp.cbegin();
    uint32_t num = pht::BitvectorUtils::decodeNumber(iter, tmp.cend(),BitvectorUtils::NumberEncoding::BINARY);
    file.write(reinterpret_cast<char*>(&num), 1);
}

Bitvector HypersuccinctTreeOutput::readBitvectorFromFile(std::ifstream &file) {
    Bitvector bitvector;
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
    for(unsigned char cNum : buffer) {
        Bitvector temp;
        BitvectorUtils::encodeNumber(std::inserter(temp, temp.end()), cNum, BitvectorUtils::NumberEncoding::BINARY);
        if(temp.size() < 8) {
            std::insert_iterator<Bitvector> iterator = std::inserter(temp, temp.begin());
            while(temp.size()<8) {
                iterator.operator=(false);
            }
        }
        ListUtils::combine(bitvector,temp);
        temp.clear();
    }
    return bitvector;
}
