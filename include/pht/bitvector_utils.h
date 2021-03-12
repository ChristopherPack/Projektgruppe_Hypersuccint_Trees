//
// Created by Christopher Pack on 25.02.2021.
//

#ifndef PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
#define PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H

#include <iostream>
#include <iterator>
#include <set>
#include <vector>

#include "unordered_tree.h"
#include "list_utils.h"
#include "hypersuccinct_tree.h"

namespace pht{
    typedef std::vector<bool> Bitvector;
    /**
     * Provides bitvector utility functions.
     *
     * This class provides functionality for the std::vector<bool> construct,
     * which is not implemented or very verbose in the standard library.
     * Functionality concerns specific encodings for the implemented Farzan Munro Algorithm.
     *
     */
    class Bitvector_Utils {
    public:
        /**
         * This Class creates a Bitvector from a List of given UnorderedTrees
         * It is supposed to use a List of MicroTrees from the Farzan Munro Algorithm
         * Encoding consists of Elias Gamma Code of the size of the MicroTrees and their structure in Balanced Parenthesis form.
         *
         * @tparam T Class implemented in UnorderedTree Todo: Check usage
         * @param fmMicroTrees List of UnorderedTrees (MicroTrees)
         * @return std::vector<bool>
         */
        template<class T> static Bitvector createBitVectorforMicroTrees(std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees) {
            //determine size of Bitvector
            Bitvector res;
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                createBitVectorForMicroTree(res,fmMicroTree);
            }
            return res;
        }

        /**
         * This Class modifies a Bitvector with a given UnorderedTrees
         * It is supposed to use a MicroTree from the Farzan Munro Algorithm
         *
         * @tparam T Class implemented in UnorderedTree Todo: Check usage
         * @param vector The Bitvector to be modified
         * @param microTree The UnorderedTree (MicroTree)
         * @return std::vector<bool>    todo: check return necessity
         */
        template<class T> static void createBitVectorForMicroTree(Bitvector& vector,std::shared_ptr<pht::UnorderedTree<T>> microTree) {
            int32_t size = microTree->getSize();
            //elias gamma code SIZE
            createEliasGamma(vector,size);
            //BP FORM in bitform
            Bitvector bp = microTree->toBalancedParenthesis();
            pht::ListUtils::addAll(vector,bp);
        }

        /**
         * This class creates EliasGamma Code for a given Number and adds it to a Bitvector
         * It is supposed to encode the size of a tree from the Farzan Munro Algorithm
         *
         * @tparam T Class implemented in UnorderedTree Todo: Check usage
         * @param vector The Bitvector to be modified
         * @param size The size to encode
         * @return std::vector<bool>    todo: check return necessity
         */
        static void createEliasGamma(Bitvector& vector, const uint32_t size) {
            int32_t logSize = floor((log2(size)));
            for(int i=0; i<logSize;i++) {
                vector.push_back(false);
            }
            for(int i =0; i<logSize+1;i++) {
                vector.push_back((size>>(logSize-i))%2==1);
            }
        }

        template<class I> static uint32_t decodeEliasGamma(I& iterator) {//iterator) {
            if(*iterator == false) {
                uint32_t size = 0;
                while(*iterator == false) {
                    size++;
                    iterator++;
                }
                Bitvector num;
                for(int j =0; j<=size; j++) {
                    num.push_back(*iterator);
                    iterator++;
                }
                return bitvectorToNumber(num);
            }
            iterator++;
            return 1;
        }

        template<class T> static void createMicroInterconnections(std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree, std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees, uint32_t size,MiniTree& miniTree) {
            uint32_t dummySize = floor(log2(2*size+1))+1;
            assert(fmMiniTree->getRoot() == fmMicroTrees.at(0)->getRoot());
            std::vector<std::shared_ptr<pht::Node<T>>> rootNodes;
            ListUtils::map(fmMicroTrees,rootNodes, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getRoot();});
            std::vector<std::shared_ptr<pht::Node<T>>> distinctRootNodes;
            ListUtils::distinct(rootNodes, distinctRootNodes);
            std::vector<std::shared_ptr<pht::Node<T>>> firstChildren;
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> filteredTrees = fmMicroTrees;
            ListUtils::filter(filteredTrees, [](std::shared_ptr<UnorderedTree<T>> x){return !(x -> isLeaf(x->getRoot()));});
            ListUtils::map(filteredTrees,firstChildren, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getDirectDescendants(x->getRoot()).at(x->getDirectDescendants(x->getRoot()).size()-1);});
            //todo: zählung von firstChildren ist front - zählung in enumerate ist reversed

            //FIDs und TypeVectors
            //todo: Elias Gamma Code für FIDs
            for(std::shared_ptr<pht::Node<T>> rootNode : distinctRootNodes) {
                std::vector<std::shared_ptr<pht::Node<T>>> children = ListUtils::reverse(fmMiniTree->getDirectDescendants(rootNode));
                createEliasGamma(miniTree.FIDs, children.size());
                for(std::shared_ptr<pht::Node<T>> node : children) {
                    if(ListUtils::contains(rootNodes,node)) {
                        miniTree.FIDs.push_back(true);
                        miniTree.typeVectors.push_back(false);

                    }
                    else if(ListUtils::contains(firstChildren,node))
                    {
                        miniTree.FIDs.push_back(true);
                        miniTree.typeVectors.push_back(true);
                    }
                    else
                    {
                        miniTree.FIDs.push_back(false);
                    }
                }

            }

            //Dummy Nodes
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                bool hadDummy = false;
                for(std::shared_ptr<pht::Node<T>> node : fmMicroTree->getNodes()) {
                    if(node != fmMicroTree->getRoot()) {
                        std::vector<std::shared_ptr<pht::Node<T>>> children = fmMiniTree->getDirectDescendants(node);
                        for(int ind = 0; ind<children.size();ind++) {
                            if(!ListUtils::contains(fmMicroTree->getNodes(),children.at(ind))) {
                                std::shared_ptr<pht::Node<T>> dummyNode = std::make_shared<pht::Node<T>>(T());
                                //Index für Tree order
                                fmMicroTree->insert(dummyNode,ind, node);
                                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                                Bitvector num = numberToBitvector(fmMicroTree->enumerate(dummyNode));
                                for (int i = 0; i < dummySize-num.size(); i++) {
                                    miniTree.dummys.push_back(false);
                                }
                                ListUtils::addAll(miniTree.dummys, num);
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
                        miniTree.dummys.push_back(false);
                    }
                }
            }
        }


        template<class T> static void createMiniInterconnections(std::shared_ptr<pht::UnorderedTree<T>> tree, std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMiniTrees, uint32_t size,HypersuccinctTree<T>& hypersuccinctTree) {
            uint32_t dummySize = floor(log2(2*size+1))+1;
            assert(tree->getRoot() == fmMiniTrees.at(0)->getRoot());
            std::vector<std::shared_ptr<pht::Node<T>>> rootNodes;
            ListUtils::map(fmMiniTrees,rootNodes, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getRoot();});
            std::vector<std::shared_ptr<pht::Node<T>>> distinctRootNodes;
            ListUtils::distinct(rootNodes, distinctRootNodes);
            std::vector<std::shared_ptr<pht::Node<T>>> firstChildren;
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> filteredTrees = fmMiniTrees;
            ListUtils::filter(filteredTrees, [](std::shared_ptr<UnorderedTree<T>> x){return !(x -> isLeaf(x->getRoot()));});
            ListUtils::map(filteredTrees,firstChildren, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getDirectDescendants(x->getRoot()).at(x->getDirectDescendants(x->getRoot()).size()-1);});
            //todo: zählung von firstChildren ist front - zählung in enumerate ist reversed

            //FIDs und TypeVectors
            //todo: Elias Gamma Code für FIDs
            for(std::shared_ptr<pht::Node<T>> rootNode : distinctRootNodes) {
                std::vector<std::shared_ptr<pht::Node<T>>> children = ListUtils::reverse(tree->getDirectDescendants(rootNode));
                createEliasGamma(hypersuccinctTree.FIDs, children.size());
                for(std::shared_ptr<pht::Node<T>> node : children) {
                    if(ListUtils::contains(rootNodes,node)) {
                        hypersuccinctTree.FIDs.push_back(true);
                        hypersuccinctTree.typeVectors.push_back(false);

                    }
                    else if(ListUtils::contains(firstChildren,node))
                    {
                        hypersuccinctTree.FIDs.push_back(true);
                        hypersuccinctTree.typeVectors.push_back(true);
                    }
                    else
                    {
                        hypersuccinctTree.FIDs.push_back(false);
                    }
                }

            }
            /*
            //Dummy Nodes
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                bool hadDummy = false;
                for(std::shared_ptr<pht::Node<T>> node : fmMicroTree->getNodes()) {
                    if(node != fmMicroTree->getRoot()) {
                        std::vector<std::shared_ptr<pht::Node<T>>> children = fmMiniTree->getDirectDescendants(node);
                        for(int ind = 0; ind<children.size();ind++) {
                            if(!ListUtils::contains(fmMicroTree->getNodes(),children.at(ind))) {
                                std::shared_ptr<pht::Node<T>> dummyNode = std::make_shared<pht::Node<T>>(T());
                                //Index für Tree order
                                fmMicroTree->insert(dummyNode,ind, node);
                                Bitvector num = numberToBitvector(fmMicroTree->enumerate(dummyNode));
                                for(bool bit: num) {
                                    std::cout << bit;
                                }
                                for (int i = 0; i < dummySize-num.size(); i++) {
                                    miniTree.dummys.push_back(false);
                                }
                                ListUtils::addAll(miniTree.dummys, num);
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
                        miniTree.dummys.push_back(false);
                    }
                }
            }*/
        }

        static Bitvector numberToBitvector(uint32_t num) {
            Bitvector res;
            if (num == 0) {
                res.push_back(false);
                return res;
            }
            int size = floor(log2(num)) + 1;
            for(uint32_t i = 0; i<size; i++) {
                res.push_back((num>>(size-1-i))&1);
            }
            return res;
        }

        static uint32_t bitvectorToNumber(Bitvector bitvector) {
            uint32_t res = 0;
            for(uint32_t i = 0; i<bitvector.size(); i++) {
                res <<= 1;
                res = res | (bitvector.at(i)?1:0);
            }
            return res;
        }

        static Bitvector getMicroTree(Bitvector bitvector,uint32_t index) {
            return findEliasGammaIndex(bitvector,index,2);
        }

        static Bitvector getMicroFID(Bitvector bitvector,uint32_t index) {
            return findEliasGammaIndex(bitvector,index,1);
        }

        static Bitvector findEliasGammaIndex(Bitvector& bitvector, uint32_t index,uint32_t multiplier) {
            //iterator
            Bitvector::iterator iterator = bitvector.begin();
            //for index:
            for(int i=0;i<index;i++) {
                //decode Elias Gamma
                int length = decodeEliasGamma(iterator)*multiplier;
                //Count until Elias Gamma is done / skip this FID
                iterator+=length;
            }
            //when index is found:
            //decode Elias Gamma
            int length = decodeEliasGamma(iterator)*multiplier;
            //return until Elias Gamma is done / Return this FID
            Bitvector micro;
            for(int j =0; j<length; j++) {
                micro.push_back(*iterator);
                iterator++;
            }
            return micro;
        }

        static Bitvector getMicroTypeVector(Bitvector bitvector,const Bitvector& fids,uint32_t index) {
            //iterator
            //skip these TypeVectors
            Bitvector::iterator iterator = bitvector.begin();
            for(int i=0; i<index;i++) {
                uint32_t indexLength = getTypeVectorLength(fids,i);
                iterator+=indexLength;
            }

            //return typeVector
            uint32_t indexLength = getTypeVectorLength(fids,index);
            Bitvector typeV;
            for(int j =0;j<indexLength; j++) {
                typeV.push_back(*iterator);
                iterator++;
            }
            return typeV;
        }

        static uint32_t getTypeVectorLength(const Bitvector& fids, uint32_t index) {
            Bitvector fid;
            fid = getMicroFID(fids, index);
            int indexLength = 0;
            for(int j=0; j<fid.size();j++) {
                if(fid.at(j)) {
                    indexLength++;
                }
            }
            return indexLength;
        }

        static Bitvector getMicroDummys(Bitvector bitvector,uint32_t index, uint32_t size) {
            uint32_t dummySize = floor(log2(2*size+1))+1;
            Bitvector::iterator iterator = bitvector.begin();
            iterator+=(dummySize*index);
            Bitvector dummy;
            for(int j =0;j<dummySize; j++) {
                dummy.push_back(*iterator);
                iterator++;
            }
            return dummy;
        }
    };
}


#endif //PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
