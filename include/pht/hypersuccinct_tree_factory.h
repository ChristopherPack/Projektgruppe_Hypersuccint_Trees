//
// Created by Christopher Pack on 15.01.2021.
//

#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H

#include <iostream>
#include <iterator>
#include <map>
#include <set>

#include "huffman.h"
#include "hypersuccinct_tree.h"
#include "hst_output.h"

namespace pht {
    /**
     * Creates Hypersuccinct Code for a given Unordered Tree utilizing the Farzan Munro Algorithm.
     */
    class HypersuccinctTreeFactory {
    public:
        /**
         * Creates Hypersuccinct Code for a given Unordered Tree utilizing the Farzan Munro Algorithm.
         *
         * @tparam T Class implemented in UnorderedTree.
         * @param tree the UnorderedTree to be encoded.
         * @return HypersuccinctTree class representing the Hypersuccinct code todo: see Class
         */
        template<class T> static HypersuccinctTree create(const std::shared_ptr<UnorderedTree<T>> tree, bool huffman = false) {
            HypersuccinctTree hypersuccinctTree;

            #ifdef PHT_TEST
            uint32_t sizeMini = 12;
            uint32_t sizeMicro = 4;
            #else
            uint32_t sizeMini = ceil(pow(log2(tree->getSize()), 2.0));
            uint32_t sizeMicro = ceil((log2(tree->getSize())) / 8.0);
            #endif

            hypersuccinctTree.miniSize = pht::Bitvector_Utils::numberToBitvector(sizeMini);
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMiniTrees = pht::FarzanMunro<T>::decompose(tree, sizeMini);

            /*for(std::shared_ptr<pht::UnorderedTree<std::string>>& fmMiniTree : fmMiniTrees) {

                std::cout << "Size of MiniTree: " << fmMiniTree->getSize() << "\n";
                std::cout << "Root of MiniTree: " << fmMiniTree->getRoot()->getValue() << "\n";
                std::cout << "Nodes of MiniTree: " << *fmMiniTree << "\n";
            }*/

            hypersuccinctTree.microSize = pht::Bitvector_Utils::numberToBitvector(sizeMicro);

            std::tuple<Bitvector,Bitvector> miniIntercon = create1_2_Interconnections(tree,fmMiniTrees,sizeMini);
            hypersuccinctTree.miniFIDs = std::get<0>(miniIntercon);
            hypersuccinctTree.miniTypeVectors = std::get<1>(miniIntercon);

            Bitvector miniDummys = createDummyInterconnections(tree,fmMiniTrees,sizeMini);
            hypersuccinctTree.miniDummys = miniDummys;

            std::map<std::vector<bool>,uint32_t> bpsAndOccurrences;

            for(std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree : fmMiniTrees) {

                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees = pht::FarzanMunro<T>::decompose(fmMiniTree, sizeMicro);
                MiniTree miniTree = MiniTree();

                std::tuple<Bitvector,Bitvector> microIntercon = create1_2_Interconnections(fmMiniTree,fmMicroTrees,sizeMicro);
                miniTree.FIDs = std::get<0>(microIntercon);
                miniTree.typeVectors = std::get<1>(microIntercon);
                Bitvector dummys = createDummyInterconnections(fmMiniTree,fmMicroTrees,sizeMicro);
                miniTree.dummys = dummys;

                for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                    std::vector<bool> bp = fmMicroTree->toBalancedParenthesis();
                    if(huffman) {
                        if(bpsAndOccurrences.find(bp) == bpsAndOccurrences.end()) {
                            bpsAndOccurrences.insert({bp, 0});
                        }
                        bpsAndOccurrences.at(bp)++;
                    }
                    MicroTreeData microTreeData(bp);
                    if(!ListUtils::containsAny(hypersuccinctTree.lookupTable, {microTreeData})) {
                        hypersuccinctTree.lookupTable.push_back(microTreeData);
                    }
                }
                miniTree.microTrees = pht::Bitvector_Utils::createBitVectorforMicroTrees(fmMicroTrees);
                hypersuccinctTree.miniTrees.push_back(miniTree);
            }

            if(huffman) {
                std::map<std::vector<bool>,std::vector<bool>> huffmanTable = Huffman::generateTable<std::vector<bool>>(bpsAndOccurrences);
                convertToHuffman(hypersuccinctTree, huffmanTable);
            }

            return hypersuccinctTree;
        }

        static void convertToHuffman(HypersuccinctTree& tree, std::map<std::vector<bool>,std::vector<bool>> huffmanTable) {
            for(MicroTreeData& x : tree.lookupTable) {
                x.bp = x.index;
                x.index = huffmanTable.at(x.index);
            }
            for(MiniTree& x : tree.miniTrees) {
                std::vector<bool> oldEncodedMicros = x.microTrees;
                x.microTrees.clear();
                uint32_t entryCount = Bitvector_Utils::getEGEntryCount(oldEncodedMicros, 2);
                for(uint32_t i = 0; i < entryCount; i++) {
                    std::vector<bool> bp = Bitvector_Utils::getBitvectorAtIndexEG(oldEncodedMicros, i, 2);
                    ListUtils::combine(x.microTrees, huffmanTable.at(bp));
                }
                HypersuccinctTreeVisualizer::printBitvector(x.microTrees);
            }
        }

        /**
         *
         * @tparam T
         * @param baseTree
         * @param subtrees
         * @param size
         * @return
         */
        template<class T> static std::tuple<Bitvector,Bitvector> create1_2_Interconnections(std::shared_ptr<pht::UnorderedTree<T>> baseTree, std::vector<std::shared_ptr<pht::UnorderedTree<T>>> subtrees, uint32_t size) {
            Bitvector FIDs;
            Bitvector typeVectors;
            uint32_t dummySize = floor(log2(2*size+1))+1;
            std::vector<std::shared_ptr<pht::Node<T>>> rootNodes = ListUtils::mapped<std::shared_ptr<UnorderedTree<T>>, std::shared_ptr<pht::Node<T>>>(subtrees, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getRoot();});
            std::vector<std::shared_ptr<pht::Node<T>>> distinctRootNodes = ListUtils::distincted(rootNodes);
            std::vector<std::shared_ptr<pht::Node<T>>> firstChildren;
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> filteredTrees = ListUtils::filtered<std::shared_ptr<UnorderedTree<T>>>(subtrees, [](std::shared_ptr<UnorderedTree<T>> x){return !(x -> isLeaf(x->getRoot()));});
            firstChildren = ListUtils::mapped<std::shared_ptr<UnorderedTree<T>>, std::shared_ptr<pht::Node<T>>>(filteredTrees, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getDirectDescendants(x->getRoot()).at(0);});
            //zählung von firstChildren ist front - zählung in enumerate

            //FIDs und TypeVectors
            for(std::shared_ptr<pht::Node<T>> rootNode : distinctRootNodes) {
                std::vector<std::shared_ptr<pht::Node<T>>> children = baseTree->getDirectDescendants(rootNode);
                pht::Bitvector_Utils::createEliasGamma(FIDs, children.size());
                for(std::shared_ptr<pht::Node<T>> node : children) {
                    if(ListUtils::containsAny(rootNodes,{node})) {
                        FIDs.push_back(true);
                        typeVectors.push_back(false);
                    }
                    else if(ListUtils::containsAny(firstChildren,{node}))
                    {
                        FIDs.push_back(true);
                        typeVectors.push_back(true);
                    }
                    else
                    {
                        FIDs.push_back(false);
                    }
                }

            }
            return {FIDs, typeVectors};
        }

        /**
         *
         * @tparam T
         * @param baseTree
         * @param subtrees
         * @param size
         * @return
         */
        template<class T> static Bitvector createDummyInterconnections(std::shared_ptr<pht::UnorderedTree<T>> baseTree, std::vector<std::shared_ptr<pht::UnorderedTree<T>>> subtrees, uint32_t size) {
            Bitvector dummys;
            uint32_t dummySize = floor(log2(2*size+1))+1;
            //Dummy Nodes
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : subtrees) {
                bool hadDummy = false;
                for(std::shared_ptr<pht::Node<T>> node : fmMicroTree->getNodes()) {
                    if(node != fmMicroTree->getRoot()) {
                        std::vector<std::shared_ptr<pht::Node<T>>> children = baseTree->getDirectDescendants(node);
                        for(int ind = 0; ind<children.size();ind++) {
                            if(!ListUtils::containsAny(fmMicroTree->getNodes(),{children.at(ind)})) {
                                std::shared_ptr<pht::Node<T>> dummyNode = std::make_shared<pht::Node<T>>(T());
                                //Index für Tree order
                                fmMicroTree->insert(dummyNode,ind, node);
                                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                                Bitvector num = pht::Bitvector_Utils::numberToBitvector(fmMicroTree->enumerate(dummyNode));
                                for (int i = 0; i < dummySize-num.size(); i++) {
                                    dummys.push_back(false);
                                }
                                ListUtils::combine(dummys, num);
                                hadDummy = true;
                                break;
                            }
                        }
                        if(hadDummy) {
                            break;
                        }
                    }
                }
                if(!hadDummy) {
                    for (int i = 0; i < dummySize; i++) {
                        dummys.push_back(false);
                    }
                }
            }
            return dummys;
        }
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H