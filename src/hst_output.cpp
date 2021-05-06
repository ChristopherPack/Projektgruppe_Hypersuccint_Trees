#include <iostream>
#include <fstream>
#include "pht/hypersuccinct_tree.h"
#include "pht/hst_output.h"


using namespace pht;
using namespace std;

/**
 * todo: include printing the Lookup Table
 * @param tree
 */
void HypersuccinctTreeVisualizer::printTree(HypersuccinctTree &tree) {
    cout << "Hypersuccinct Tree:" << endl;
    cout << "MiniSize:  ";
    printBitvector(tree.getMiniSize());
    cout << "MicroSize:  ";
    printBitvector(tree.getMicroSize());
    cout << endl;

    cout << "Amount of MiniTrees: " << tree.getMiniTrees().size() << endl;
    cout << "MiniFIDs:  ";
    printBitvector(tree.getMiniFIDs());
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
    }
}

void HypersuccinctTreeVisualizer::printBitvector(const vector<bool>& bitvector) {
    for(bool bit: bitvector) {
        cout << bit;
    }
    cout << endl;
}

string HypersuccinctTreeVisualizer::splitFIDs(const vector<bool> &bitvector, const string &separator) {
    auto iterator = bitvector.begin();
    string result;

    while(iterator != bitvector.end()){
        uint32_t eg = 0;
        if(!*iterator) {
            uint32_t size = 0;
            while(!*iterator) {
                size++;
                result.append(to_string(*iterator));
                iterator++;
            }
            Bitvector num;
            for(int j =0; j<=size; j++) {
                num.push_back(*iterator);
                result.append(to_string(*iterator));
                iterator++;
            }
            auto iter = num.cbegin();
            eg = pht::Bitvector_Utils::decodeNumber(iter, num.cend(),Bitvector_Utils::NumberEncoding::BINARY);
        } else{
            result.append(to_string(*iterator));
            iterator++;
            eg = 1;
        }
        for (int i = eg; i > 1; i--){
            result.append(to_string(*iterator));
            iterator++;
        }
        result.append(to_string(*iterator));
        result.append(separator);
        if(iterator == bitvector.end()){
            break;
        }
        iterator++;
    };
    return result;
}

void HypersuccinctTreeVisualizer::writeBitvector(std::ofstream &file, Bitvector& bitvector) {
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

void HypersuccinctTreeVisualizer::writeToFile(HypersuccinctTree &tree) {
    //todo: implementing some sort of file explorer would be nice
    //todo: need to think about how to make the bitvector
    //encode mit trennzeichen: 10000000000000000000000000000001 (30 nullen)
    std::ofstream file;
    file.open("tree.txt", std::ofstream::binary);
    Bitvector fileBitvector;
    Bitvector_Utils::encodeNumber(fileBitvector,Bitvector_Utils::decodeNumber(tree.getMiniSize(),Bitvector_Utils::NumberEncoding::BINARY),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    Bitvector_Utils::encodeNumber(fileBitvector,Bitvector_Utils::decodeNumber(tree.getMicroSize(),Bitvector_Utils::NumberEncoding::BINARY),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    Bitvector_Utils::encodeNumber(fileBitvector, tree.getMiniTrees().size(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    Bitvector_Utils::encodeNumber(fileBitvector, tree.getLookupTable().size(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
    ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
    Bitvector temp = addDuplicateSeparator(tree.getMiniFIDs(),"10000000000000000000000000000001");
    ListUtils::combine(fileBitvector, temp);
    ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
    temp.clear();
    temp = addDuplicateSeparator(tree.getMiniTypeVectors(),"10000000000000000000000000000001");
    ListUtils::combine(fileBitvector, temp);
    ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
    temp = addDuplicateSeparator(tree.getMiniDummys(),"10000000000000000000000000000001");
    ListUtils::combine(fileBitvector, temp);
    ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
    for(MiniTree& miniTree : tree.getMiniTrees()) {
        temp = addDuplicateSeparator(miniTree.FIDs, "10000000000000000000000000000001");
        ListUtils::combine(fileBitvector, temp);
        ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
        temp = addDuplicateSeparator(miniTree.typeVectors, "10000000000000000000000000000001");
        ListUtils::combine(fileBitvector, temp);
        ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
        temp = addDuplicateSeparator(miniTree.dummys, "10000000000000000000000000000001");
        ListUtils::combine(fileBitvector, temp);
        ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
        temp = addDuplicateSeparator(miniTree.microTrees, "10000000000000000000000000000001");
        ListUtils::combine(fileBitvector, temp);
        ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
        temp = addDuplicateSeparator(miniTree.rootAncestors, "10000000000000000000000000000001");
        ListUtils::combine(fileBitvector, temp);
        ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
        temp = addDuplicateSeparator(miniTree.dummyAncestors, "10000000000000000000000000000001");
        ListUtils::combine(fileBitvector, temp);
        ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
    }
    for(MicroTreeData& microTreeData : tree.getLookupTable()) {
        temp = addDuplicateSeparator(microTreeData.index, "10000000000000000000000000000001");
        ListUtils::combine(fileBitvector, temp);
        ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
        temp = addDuplicateSeparator(microTreeData.bp, "10000000000000000000000000000001");
        ListUtils::combine(fileBitvector, temp);
        ListUtils::combine(fileBitvector, Bitvector_Utils::convertToBitvector("10000000000000000000000000000001"));
    }
    writeBitvector(file,fileBitvector);
    file.close();
}

Bitvector HypersuccinctTreeVisualizer::addDuplicateSeparator(const Bitvector& bitvector, const string& separator) {
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

HypersuccinctTree HypersuccinctTreeVisualizer::readFromFile(string path) {
    HypersuccinctTree hst;

    return hst;
}
