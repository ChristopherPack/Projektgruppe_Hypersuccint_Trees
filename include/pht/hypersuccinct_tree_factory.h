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

            pht::Bitvector_Utils::encodeNumber(std::inserter(hypersuccinctTree.miniSize, hypersuccinctTree.miniSize.begin()), sizeMini,Bitvector_Utils::NumberEncoding::BINARY);
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMiniTrees = pht::FarzanMunro<T>::decompose(tree, sizeMini);

            /*for(std::shared_ptr<pht::UnorderedTree<std::string>>& fmMiniTree : fmMiniTrees) {

                std::cout << "Size of MiniTree: " << fmMiniTree->getSize() << "\n";
                std::cout << "Root of MiniTree: " << fmMiniTree->getRoot()->getValue() << "\n";
                std::cout << "Nodes of MiniTree: " << *fmMiniTree << "\n";
            }*/

            pht::Bitvector_Utils::encodeNumber(std::inserter(hypersuccinctTree.microSize, hypersuccinctTree.microSize.begin()), sizeMicro,Bitvector_Utils::NumberEncoding::BINARY);

            std::tuple<Bitvector,Bitvector> miniIntercon = create1_2_Interconnections(tree,fmMiniTrees,sizeMini);
            hypersuccinctTree.miniFIDs = std::get<0>(miniIntercon);
            hypersuccinctTree.miniTypeVectors = std::get<1>(miniIntercon);

            Bitvector miniDummys = createDummyInterconnections(tree,fmMiniTrees,sizeMini);
            hypersuccinctTree.miniDummys = miniDummys;

            std::map<std::vector<bool>,uint32_t> bpsAndOccurrences;

            for(std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree : fmMiniTrees) {
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees = pht::FarzanMunro<T>::decompose(fmMiniTree, sizeMicro);
                MiniTree miniTree = MiniTree();
                if(fmMiniTree->hasDummy()) {
                    for(const std::shared_ptr<pht::UnorderedTree<T>>& fmMicroTree : fmMicroTrees) {
                        miniTree.rootAncestors.push_back(fmMiniTree->isAncestor(fmMiniTree->getDummy(), fmMicroTree->getRoot()));
                    }
                }

                std::tuple<Bitvector,Bitvector> microIntercon = create1_2_Interconnections(fmMiniTree,fmMicroTrees,sizeMicro);
                miniTree.FIDs = std::get<0>(microIntercon);
                miniTree.typeVectors = std::get<1>(microIntercon);
                Bitvector dummys = createDummyInterconnections(fmMiniTree, fmMicroTrees, sizeMicro);
                miniTree.dummys = dummys;
                if(fmMiniTree->hasDummy()) {
                    for(const std::shared_ptr<pht::UnorderedTree<T>>& fmMicroTree : fmMicroTrees) {
                        if(fmMicroTree->hasDummy()) {
                            miniTree.dummyAncestors.push_back(fmMiniTree->isAncestor(fmMiniTree->getDummy(), fmMicroTree->getDummy()));
                        } else {
                            miniTree.dummyAncestors.push_back(false);
                        }
                    }
                }

                for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                    std::vector<bool> bp = fmMicroTree->toBalancedParenthesis();
                    if(huffman) {
                        if(bpsAndOccurrences.find(bp) == bpsAndOccurrences.end()) {
                            bpsAndOccurrences.insert({bp, 0});
                        }
                        bpsAndOccurrences.at(bp)++;
                    }
                    std::vector<bool> matrix;
                    std::vector<std::shared_ptr<pht::Node<T>>> orderedNodes = fmMicroTree->getNodes();
                    ListUtils::sort<std::shared_ptr<pht::Node<T>>>(orderedNodes, [&fmMicroTree](std::shared_ptr<pht::Node<T>> a, std::shared_ptr<pht::Node<T>> b){ return fmMicroTree->enumerate(a) < fmMicroTree->enumerate(b); });
                    for(std::shared_ptr<pht::Node<T>> node1 : orderedNodes) {
                        for(std::shared_ptr<pht::Node<T>> node2 : orderedNodes) {
                            matrix.push_back(fmMicroTree->isAncestor(node2, node1));
                        }
                    }
                    MicroTreeData microTreeData(bp, matrix);
                    if(!ListUtils::containsAny(hypersuccinctTree.lookupTable, {microTreeData})) {
                        hypersuccinctTree.lookupTable.push_back(microTreeData);
                    }
                }
                miniTree.microTrees = createBitVectorforMicroTrees(fmMicroTrees);
                hypersuccinctTree.miniTrees.push_back(miniTree);
            }

            if(huffman) {
                std::map<std::vector<bool>,std::vector<bool>> huffmanTable = Huffman::generateTable<std::vector<bool>>(bpsAndOccurrences);
                convertToHuffman(hypersuccinctTree, huffmanTable);
            }

            //generateQueryData(hypersuccinctTree,fmMiniTrees);

            return hypersuccinctTree;
        }

        /**
         *
         * @param fullBitvector
         * @return
         */
        static HypersuccinctTree createFromFile(Bitvector fullBitvector) {
            HypersuccinctTree hst;
            auto iter = fullBitvector.begin();
            uint32_t miniSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            uint32_t microSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            uint32_t miniTreesSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            uint32_t lookupTableSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

            uint32_t tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            for(uint32_t i=0; i<tempSize; i++) {
                hst.miniFIDs.push_back(*iter);
                iter++;
            }

            tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            for(uint32_t i=0; i<tempSize; i++) {
                hst.miniTypeVectors.push_back(*iter);
                iter++;
            }

            tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            for(uint32_t i=0; i<tempSize; i++) {
                hst.miniDummys.push_back(*iter);
                iter++;
            }

            for(uint32_t j=0; j<miniTreesSize; j++) {
                MiniTree mini;
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    mini.FIDs.push_back(*iter);
                    iter++;
                }
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    mini.typeVectors.push_back(*iter);
                    iter++;
                }
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    mini.dummys.push_back(*iter);
                    iter++;
                }
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    mini.microTrees.push_back(*iter);
                    iter++;
                }
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    mini.rootAncestors.push_back(*iter);
                    iter++;
                }
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    mini.dummyAncestors.push_back(*iter);
                    iter++;
                }
                hst.miniTrees.push_back(mini);
            }

            for(uint32_t j=0; j<lookupTableSize; j++) {
                Bitvector index;
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    index.push_back(*iter);
                    iter++;
                }
                Bitvector bp; //TODO: How to check if huffman is even used?
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    bp.push_back(*iter);
                    iter++;
                }
                Bitvector matrix;
                tempSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(uint32_t i=0; i<tempSize; i++) {
                    matrix.push_back(*iter);
                    iter++;
                }
                MicroTreeData microTreeData(index, matrix);
            }
            return hst;
        }

        template<class T> static void generateQueryData(HypersuccinctTree& tree,std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree) {

        }

        /**
         * This Class creates a Bitvector from a List of given UnorderedTrees
         * It is supposed to use a List of MicroTrees from the Farzan Munro Algorithm
         * Encoding consists of Elias Gamma Code of the size of the MicroTrees and their structure in Balanced Parenthesis form.
         *
         * @tparam T Class implemented in UnorderedTree
         * @param fmMicroTrees List of UnorderedTrees (MicroTrees)
         */
        template<class T> static Bitvector createBitVectorforMicroTrees(std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees) {
            //determine size of Bitvector
            Bitvector res;
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                int32_t size = fmMicroTree->getSize();
                //elias gamma code SIZE
                pht::Bitvector_Utils::encodeNumber(std::inserter(res, res.end()), size,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                //BP FORM in bitform
                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                pht::ListUtils::combine(res,bp);
            }
            return res;
        }

        static void convertToHuffman(HypersuccinctTree& tree, std::map<std::vector<bool>,std::vector<bool>> huffmanTable) {
            for(MicroTreeData& x : tree.lookupTable) {
                x.bp = x.index;
                x.index = huffmanTable.at(x.index);
            }
            for(MiniTree& x : tree.miniTrees) {
                std::vector<bool> oldEncodedMicros = x.microTrees;
                x.microTrees.clear();
                uint32_t entryCount = Bitvector_Utils::getEntryCount(oldEncodedMicros.cbegin(), oldEncodedMicros.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {2, 0, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator});
                for(uint32_t i = 0; i < entryCount; i++) {
                    auto iter = oldEncodedMicros.cbegin();
                    std::vector<bool> bp = Bitvector_Utils::getEntry(iter, i, oldEncodedMicros.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {2, 0, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator});
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
                pht::Bitvector_Utils::encodeNumber(std::inserter(FIDs, FIDs.end()), children.size(),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
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
                        for(int ind = 0; ind < children.size(); ind++) {
                            if(!ListUtils::containsAny(fmMicroTree->getNodes(),{children.at(ind)})) {
                                std::shared_ptr<pht::Node<T>> dummyNode = std::make_shared<pht::Node<T>>(T());
                                dummyNode->setMiniDummy();
                                //Index für Tree order
                                fmMicroTree->insert(dummyNode, ind, node);
                                fmMicroTree->setDummy(dummyNode);
                                baseTree->insert(dummyNode, ind, node);
                                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                                Bitvector num;
                                pht::Bitvector_Utils::encodeNumber(std::inserter(num, num.end()), fmMicroTree->enumerate(dummyNode),Bitvector_Utils::NumberEncoding::BINARY);
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