#include <iostream>
#include <fstream>
#include "pht/hypersuccinct_tree.h"
#include "pht/hst_output.h"


using namespace pht;
using namespace std;

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
        int eg = 0;
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
            eg = Bitvector_Utils::bitvectorToNumber(num);
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

void writeBitvector(std::ofstream &file, Bitvector bitvector) {
    for(bool bit: bitvector) {
        file << bit;
    }
}

void HypersuccinctTreeVisualizer::writeToFile(HypersuccinctTree &tree) {
    //todo: implementing some sort of file explorer would be nice
    //todo: need to think about how to make the bitvector
    std::ofstream file;
    file.open("tree.txt");
    Bitvector fileBitvector;
    Bitvector_Utils::createEliasGamma(fileBitvector, Bitvector_Utils::bitvectorToNumber(tree.getMiniSize()));
    Bitvector_Utils::createEliasGamma(fileBitvector, Bitvector_Utils::bitvectorToNumber(tree.getMicroSize()));
    Bitvector_Utils::createEliasGamma(fileBitvector, tree.getMiniFIDs().size());
    ListUtils::combine(fileBitvector, tree.getMiniFIDs());
    Bitvector_Utils::createEliasGamma(fileBitvector, tree.getMiniTypeVectors().size());
    ListUtils::combine(fileBitvector, tree.getMiniTypeVectors());
    Bitvector_Utils::createEliasGamma(fileBitvector, tree.getMiniFIDs().size());
    ListUtils::combine(fileBitvector, tree.getMiniFIDs());
    for(MiniTree &miniTree: tree.getMiniTrees()) {
        Bitvector_Utils::createEliasGamma(fileBitvector, miniTree.FIDs.size());
        ListUtils::combine(fileBitvector, miniTree.FIDs);
        Bitvector_Utils::createEliasGamma(fileBitvector, miniTree.typeVectors.size());
        ListUtils::combine(fileBitvector, miniTree.typeVectors);
        Bitvector_Utils::createEliasGamma(fileBitvector, miniTree.dummys.size());
        ListUtils::combine(fileBitvector, miniTree.dummys);
        Bitvector_Utils::createEliasGamma(fileBitvector, miniTree.microTrees.size());
        ListUtils::combine(fileBitvector, miniTree.microTrees);
    }
    for(MicroTreeData &microTreeData: tree.getLookupTable()) {
        Bitvector_Utils::createEliasGamma(fileBitvector, microTreeData.index.size());
        ListUtils::combine(fileBitvector, microTreeData.index);
    }
    writeBitvector(file,fileBitvector);
    file.close();
}

HypersuccinctTree HypersuccinctTreeVisualizer::readFromFile(string path) {
    HypersuccinctTree hst;

    return hst;
}
