//
// Created by Christopher Pack on 15.01.2021.
//

#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H

#include <iostream>
#include <iterator>
#include <set>

#include "hypersuccinct_tree.h"

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
        template<class T> static HypersuccinctTree<T> create(const std::shared_ptr<UnorderedTree<T>> tree) {
            HypersuccinctTree<T> hypersuccinctTree;

            uint32_t sizeMini = ceil(pow(log2(tree->getSize()), 2.0));

            hypersuccinctTree.miniSize = pht::Bitvector_Utils::numberToBitvector(sizeMini);
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMiniTrees = pht::ListUtils::reverse(pht::FarzanMunro<T>::decompose(tree, sizeMini));

            uint32_t sizeMicro = ceil((log2(tree->getSize())) / 8.0);

            hypersuccinctTree.microSize = pht::Bitvector_Utils::numberToBitvector(sizeMicro);
            std::tuple<Bitvector,Bitvector> miniIntercon = create1_2_Interconnections(tree,fmMiniTrees,sizeMini);
            hypersuccinctTree.miniFIDs = std::get<0>(miniIntercon);
            hypersuccinctTree.miniTypeVectors = std::get<1>(miniIntercon);
            Bitvector miniDummys = createDummyInterconnections(tree,fmMiniTrees,sizeMini);
            hypersuccinctTree.miniDummys = miniDummys;

            for(std::shared_ptr<pht::UnorderedTree<T>> fmMiniTree : fmMiniTrees) {

                //todo: We might need this for Interconnections
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees = pht::ListUtils::reverse(pht::FarzanMunro<T>::decompose(fmMiniTree, sizeMicro));
                MiniTree miniTree = MiniTree();

                //todo: Create Mini Dummy Nodes possible Position
                std::tuple<Bitvector,Bitvector> microIntercon = create1_2_Interconnections(fmMiniTree,fmMicroTrees,sizeMicro);
                miniTree.FIDs = std::get<0>(microIntercon);
                miniTree.typeVectors = std::get<1>(microIntercon);
                Bitvector dummys = createDummyInterconnections(fmMiniTree,fmMicroTrees,sizeMicro);
                miniTree.dummys = dummys;

                for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                    std::vector<bool> bp = fmMicroTree->toBalancedParenthesis();
                    MicroTreeData microTreeData(bp);
                    if(!ListUtils::contains(hypersuccinctTree.lookupTable, microTreeData)) {
                        hypersuccinctTree.lookupTable.push_back(microTreeData);
                    }
                }
                miniTree.microTrees = createBitVectorforMicroTrees(fmMicroTrees);
                hypersuccinctTree.miniTrees.push_back(miniTree);
            }

            return hypersuccinctTree;
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
            assert(baseTree->getRoot() == subtrees.at(0)->getRoot());
            std::vector<std::shared_ptr<pht::Node<T>>> rootNodes;
            ListUtils::map(subtrees, rootNodes, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getRoot();});
            std::vector<std::shared_ptr<pht::Node<T>>> distinctRootNodes;
            ListUtils::distinct(rootNodes, distinctRootNodes);
            std::vector<std::shared_ptr<pht::Node<T>>> firstChildren;
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> filteredTrees = subtrees;
            ListUtils::filter(filteredTrees, [](std::shared_ptr<UnorderedTree<T>> x){return !(x -> isLeaf(x->getRoot()));});
            ListUtils::map(filteredTrees,firstChildren, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getDirectDescendants(x->getRoot()).at(x->getDirectDescendants(x->getRoot()).size()-1);});
            //zählung von firstChildren ist front - zählung in enumerate ist reversed

            //FIDs und TypeVectors
            for(std::shared_ptr<pht::Node<T>> rootNode : distinctRootNodes) {
                std::vector<std::shared_ptr<pht::Node<T>>> children = ListUtils::reverse(baseTree->getDirectDescendants(rootNode));
                createEliasGamma(FIDs, children.size());
                for(std::shared_ptr<pht::Node<T>> node : children) {
                    if(ListUtils::contains(rootNodes,node)) {
                        FIDs.push_back(true);
                        typeVectors.push_back(false);

                    }
                    else if(ListUtils::contains(firstChildren,node))
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
                            if(!ListUtils::contains(fmMicroTree->getNodes(),children.at(ind))) {
                                std::shared_ptr<pht::Node<T>> dummyNode = std::make_shared<pht::Node<T>>(T());
                                //Index für Tree order
                                fmMicroTree->insert(dummyNode,ind, node);
                                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                                Bitvector num = numberToBitvector(fmMicroTree->enumerate(dummyNode));
                                for (int i = 0; i < dummySize-num.size(); i++) {
                                    dummys.push_back(false);
                                }
                                ListUtils::addAll(dummys, num);
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


        /**
         * Creates all Interconnections for MicroTrees
         * todo: There must be a way to fuse this with createMini but this requires a restructuring of HypersuccinctTree
         *
         * @tparam T implemented in UnorderedTree
         * @param fmMiniTree the MicroTrees created by FarzanMunro
         * @param fmMicroTrees the MiniTrees created by FarzanMunro
         * @param size the size of the MicroTrees
         * @param miniTree The MiniTree to save the created Bitvectors into
         */
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


        /**
         * Creates all interconnections for MiniTrees
         * todo: There must be a way to fuse this with createMicro but this requires a restructuring of HypersuccinctTree
         *
         * @tparam T implemented in UnorderedTree
         * @param tree the entire Tree as unordered Tree
         * @param fmMiniTrees the MiniTrees created by FarzanMunro
         * @param size the size of the MiniTrees
         * @param hypersuccinctTree the HypersuccinctTree to save the created bitvectors into
         */
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
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H