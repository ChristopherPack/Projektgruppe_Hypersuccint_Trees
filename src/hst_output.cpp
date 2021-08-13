#include <iostream>
#include <fstream>
#include "pht/hypersuccinct_tree.h"
#include "pht/hypersuccinct_tree_factory.h"
#include "pht/hst_output.h"


using namespace pht;
using namespace std;

void hello() {
std::cout << "Hello";
}

/**
 * todo: include printing the Lookup Table
 * @param tree
 */
void HypersuccinctTreeOutput::printTree(HypersuccinctTree &tree) {
    cout << "Hypersuccinct Tree:" << endl;
    cout << "IsHuffman:   ";
    cout << tree.isHuffman() << endl;
    cout << "MiniSize:  ";
    printBitvector(tree.getMiniSize());
    cout << "MicroSize:  ";
    printBitvector(tree.getMicroSize());
    cout << endl;

    cout << "Amount of MiniTrees: " << tree.getMiniTrees().size() << endl;
    cout << "MiniFIDs:  ";
    cout << splitFIDs(tree.getMiniFIDs(), "  ") << endl;
    cout << "MiniTypeVectors:  ";
    printBitvector(tree.getMiniTypeVectors());
    cout << "MiniDummys:  ";
    printBitvector(tree.getMiniDummys());

    pht::MiniTree miniTree = tree.getMiniTree(0);
    for(int index = 0 ; index < tree.getMiniTrees().size(); index++) {
        cout << "MicroTreesinMiniTree" << index << ":  ";
        printBitvector(tree.getMiniTree(index).microTrees);
        cout << "MicroTreeFIDs:  ";
        cout << splitFIDs(tree.getMiniTree(index).FIDs, "  ") << endl;
        cout << "MicroTreeTypeVectors:  ";
        printBitvector(tree.getMiniTree(index).typeVectors);
        cout << "MicroTreeDummys:  ";
        printBitvector(tree.getMiniTree(index).dummys);

        cout << "Mini Top FID Index:  ";
        printBitvector(tree.getMiniTree(index).miniTopFIDIndex);
        cout << "Mini Low FID Index:  ";
        printBitvector(tree.getMiniTree(index).miniLowFIDIndex);
        cout << "Micro Top FID Indices:  ";
        printBitvector(tree.getMiniTree(index).microTopFIDIndices);
        cout << "Micro Low FID Indices:  ";
        printBitvector(tree.getMiniTree(index).microLowFIDIndices);

        cout << "RootAncestors:  ";
        printBitvector(tree.getMiniTree(index).rootAncestors);
        cout << "DummyAncestors:  ";
        printBitvector(tree.getMiniTree(index).dummyAncestors);
        cout << "MiniDummyTree:  ";
        printBitvector(tree.getMiniTree(index).miniDummyTree);
        cout << "MiniDummyIndex:  ";
        printBitvector(tree.getMiniTree(index).miniDummyIndex);
        cout << "MiniDummyPointer:  ";
        printBitvector(tree.getMiniTree(index).miniDummyPointer);
        cout << "MicroDummyPointers:  ";
        printBitvector(tree.getMiniTree(index).microDummyPointers);
        cout << "Subtree Size at MiniTree Root:  ";
        printBitvector(tree.getMiniTree(index).subTree);
        cout << "SubTreeSize at MicroTree Roots:  ";
        printBitvector(tree.getMiniTree(index).microSubTrees);
        cout << "Depth at MiniTree Root:  ";
        printBitvector(tree.getMiniTree(index).miniDepth);
        cout << "Height at MiniTree Root:  ";
        printBitvector(tree.getMiniTree(index).miniHeight);
        cout << "Depth at MiniTree Dummy:  ";
        printBitvector(tree.getMiniTree(index).miniDummyDepth);
        cout << "Height at MiniTree Dummy:  ";
        printBitvector(tree.getMiniTree(index).miniDummyHeight);
        cout << "Depth at MicroTree Roots:  ";
        printBitvector(tree.getMiniTree(index).rootDepths);
        cout << "Height at MicroTree Roots:  ";
        printBitvector(tree.getMiniTree(index).rootHeights);
        cout << "Leaf Size at MiniTree Root:  ";
        printBitvector(tree.getMiniTree(index).miniLeaves);
        cout << "Leaf Size at MicroTree Roots:  ";
        printBitvector(tree.getMiniTree(index).microLeaves);
        cout << "Leftmost MiniTree:  ";
        printBitvector(tree.getMiniTree(index).miniTreeLeftmostLeafPointer);
        cout << "Rightmost MiniTree:  ";
        printBitvector(tree.getMiniTree(index).miniTreeRightmostLeafPointer);
        cout << "Leftmost MicroTree within MiniTree:  ";
        printBitvector(tree.getMiniTree(index).microTreeLeftmostLeafPointers);
        cout << "Rightmost MicroTree within MiniTree:  ";
        printBitvector(tree.getMiniTree(index).microTreeRightmostLeafPointers);

        cout << "LeafRank at MiniTree Root:  ";
        printBitvector(tree.getMiniTree(index).miniRootLeafRank);
        cout << "LeafRank at MiniTree Dummy:  ";
        printBitvector(tree.getMiniTree(index).miniDummyLeafRank);
        cout << "LeafRank at MicroTree Roots:  ";
        printBitvector(tree.getMiniTree(index).microRootLeafRanks);
        cout << "\n";
    }
    for(uint32_t index = 0; index < tree.getLookupTable().size(); index++) {
        cout << "LookupTableIndex:   ";
        printBitvector(tree.getLookupTableEntry(index).index);
        cout<< "BP for Huffman:   ";
        printBitvector(tree.getLookupTableEntry(index).bp);
        cout << "AncestorMap:   ";
        printBitvector(tree.getLookupTableEntry(index).ancestorMatrix);
        cout << "ChildMap:   ";
        printBitvector(tree.getLookupTableEntry(index).childMatrix);
        cout << "Degrees:   ";
        printBitvector(tree.getLookupTableEntry(index).degree);
        cout << "Subtrees:   ";
        printBitvector(tree.getLookupTableEntry(index).subTrees);
        cout << "Depths:   ";
        printBitvector(tree.getLookupTableEntry(index).nodeDepths);
        cout << "Heights:   ";
        printBitvector(tree.getLookupTableEntry(index).nodeHeights);
        cout << "Leaf Sizes:   ";
        printBitvector(tree.getLookupTableEntry(index).leaves);
        cout << "Leftmost Leaves:   ";
        printBitvector(tree.getLookupTableEntry(index).leftmost_leaf);
        cout << "Rightmost Leaves:   ";
        printBitvector(tree.getLookupTableEntry(index).rightmost_leaf);
        cout << "Leaf Ranks:   ";
        printBitvector(tree.getLookupTableEntry(index).leafRank);
    }
}

void HypersuccinctTreeOutput::printBitvector(const vector<bool>& bitvector) {
    for(bool bit: bitvector) {
        cout << bit;
    }
    cout << endl;
}

string HypersuccinctTreeOutput::splitFIDs(const vector<bool> &bitvector, const string &separator) {
    auto iterator = bitvector.begin();
    string result;

    //This is to eliminate 0 FIDs
    if(bitvector.size() == 1) {
        return "0";
    } else {
        while (iterator != bitvector.end()) {
            uint32_t eg = 0;
            if (!*iterator) {
                uint32_t size = 0;
                while (!*iterator) {
                    size++;
                    result.append(to_string(*iterator));
                    iterator++;
                }
                Bitvector num;
                for (int j = 0; j <= size; j++) {
                    num.push_back(*iterator);
                    result.append(to_string(*iterator));
                    iterator++;
                }
                auto iter = num.cbegin();
                eg = pht::Bitvector_Utils::decodeNumber(iter, num.cend(), Bitvector_Utils::NumberEncoding::BINARY);
            } else {
                result.append(to_string(*iterator));
                iterator++;
                eg = 1;
            }
            for (int i = eg; i > 1; i--) {
                result.append(to_string(*iterator));
                iterator++;
            }
            result.append(to_string(*iterator));
            result.append(separator);
            if (iterator == bitvector.end()) {
                break;
            }
            iterator++;
        };
        return result;
    }
}

void HypersuccinctTreeOutput::writeToFile(HypersuccinctTree &tree) {
    //todo: implementing some sort of file explorer would be nice
    //todo: need to think about how to make the bitvector
    //encode mit Elias Gamma
    std::ofstream file;
    file.open("tree.txt", std::ofstream::binary);
    Bitvector fileBitvector;
    fileBitvector.push_back(tree.isHuffman());
    Bitvector_Utils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()),Bitvector_Utils::decodeNumber(tree.getSize(),Bitvector_Utils::NumberEncoding::BINARY),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    Bitvector_Utils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()),Bitvector_Utils::decodeNumber(tree.getMiniSize(),Bitvector_Utils::NumberEncoding::BINARY),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    Bitvector_Utils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()),Bitvector_Utils::decodeNumber(tree.getMicroSize(),Bitvector_Utils::NumberEncoding::BINARY),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    Bitvector_Utils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()), tree.getMiniTrees().size(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    Bitvector_Utils::encodeNumber(std::inserter(fileBitvector, fileBitvector.end()), tree.getLookupTable().size(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    Bitvector emptySet = {false};

    createFileBitvector(tree.getMiniFIDs(), fileBitvector);
    createFileBitvector(tree.getMiniTypeVectors(), fileBitvector);
    createFileBitvector(tree.getMiniDummys(), fileBitvector);
    for(MiniTree& miniTree : tree.getMiniTrees()) {
        createFileBitvector(miniTree.FIDs, fileBitvector);
        createFileBitvector(miniTree.typeVectors, fileBitvector);
        createFileBitvector(miniTree.dummys, fileBitvector);
        createFileBitvector(miniTree.microTrees, fileBitvector);
        createFileBitvector(miniTree.rootAncestors, fileBitvector);
        createFileBitvector(miniTree.dummyAncestors, fileBitvector);
        createFileBitvector(miniTree.miniDummyTree, fileBitvector);
        createFileBitvector(miniTree.miniDummyIndex, fileBitvector);
        createFileBitvector(miniTree.miniDummyPointer, fileBitvector);
        createFileBitvector(miniTree.microDummyPointers, fileBitvector);
        createFileBitvector(miniTree.miniAnc, fileBitvector);
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
    }
    for(LookupTableEntry& microTreeData : tree.getLookupTable()) {
        createFileBitvector(microTreeData.index, fileBitvector);
        createFileBitvector(microTreeData.bp, fileBitvector);
        createFileBitvector(microTreeData.ancestorMatrix, fileBitvector);
        createFileBitvector(microTreeData.childMatrix, fileBitvector);
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

HypersuccinctTree HypersuccinctTreeOutput::readFromFile(string path) {
    std::ifstream file;
    file.open("tree.txt", std::ifstream::binary);
    Bitvector fileBitvector = readBitvectorFromFile(file);
    file.close();
    //funktion in Factory:
    return HypersuccinctTreeFactory::createFromFile(fileBitvector);
}

void HypersuccinctTreeOutput::createFileBitvector(Bitvector bitvector, Bitvector& target) {
    if(bitvector.empty()) {
        target.push_back(true);
        target.push_back(false);
    } else {
        Bitvector_Utils::encodeNumber(std::inserter(target, target.end()),
                                      bitvector.size(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
        ListUtils::combine(target, bitvector);
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
        uint32_t num = pht::Bitvector_Utils::decodeNumber(iter, tmp.cend(),Bitvector_Utils::NumberEncoding::BINARY);
        file.write(reinterpret_cast<char*>(&num), 1);
        bytes++;
    }
    tmp.clear();
    for(uint32_t i = 0; i < bitvector.size()%8; i++) {
        tmp.push_back(bitvector.at(bytes+i));
    }
    for(uint32_t i = 0; i < 8-(bitvector.size()%8); i++) {
        tmp.push_back(0);
    }
    auto iter = tmp.cbegin();
    uint32_t num = pht::Bitvector_Utils::decodeNumber(iter, tmp.cend(),Bitvector_Utils::NumberEncoding::BINARY);
    file.write(reinterpret_cast<char*>(&num), 1);
}

Bitvector HypersuccinctTreeOutput::readBitvectorFromFile(std::ifstream &file) {
    Bitvector bitvector;
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
    for(unsigned char cNum : buffer) {
        Bitvector temp;
        Bitvector_Utils::encodeNumber(std::inserter(temp, temp.end()), cNum, Bitvector_Utils::NumberEncoding::BINARY);
        if(temp.size() < 8) {
            std::insert_iterator<Bitvector> iterator = std::inserter(temp, temp.begin());
            while(temp.size()<8) {
                iterator.operator=(0);
            }
        }
        ListUtils::combine(bitvector,temp);
        temp.clear();
    }
    return bitvector;
}

Bitvector HypersuccinctTreeOutput::addDuplicateSeparator(const Bitvector& bitvector, const string& separator) {
    Bitvector temp = bitvector;
    Bitvector sep = Bitvector_Utils::convertToBitvector(separator);
    uint32_t sepNum = Bitvector_Utils::decodeNumber(sep, Bitvector_Utils::NumberEncoding::BINARY);
    std::vector<std::pair<Bitvector::const_iterator, Bitvector::const_iterator>> patternMatches = Bitvector_Utils::findMatches(temp.cbegin(),temp.cend(), separator);

    for(std::pair<Bitvector::const_iterator, Bitvector::const_iterator> match : patternMatches) {
        //TODO: This is apparently optimal - other option is to advance to the const_iter
        auto iterMin = temp.erase(match.first,match.first);
        uint32_t add = Bitvector_Utils::encodeNumber(std::inserter(temp, iterMin), sepNum, Bitvector_Utils::NumberEncoding::BINARY);
        assert(match.first+add < match.second);
    }

    return temp;
}
