//
// Created by Christopher Pack on 15.01.2021.
//

#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H



#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <mutex>

#include "logger.h"
#include "huffman.h"
#include "hypersuccinct_tree.h"
#include "hst_output.h"
#include "thread_pool.hpp"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
using namespace std;

namespace pht {
    /**
     * Creates Hypersuccinct Code for a given Unordered Tree utilizing the Farzan Munro Algorithm.
     */
    class HypersuccinctTreeFactory {
    public:
        /**
         * Creates Hypersuccinct Code for a given Unordered Tree utilizing the Farzan Munro Algorithm.
         *
         * @tparam T Class implemented in UnorderedTree
         * @param tree The UnorderedTree to be encoded
         * @param huffman If the MicroTrees should be encoded with Huffman encoding
         * @return HypersuccinctTree class representing the Hypersuccinct code
         */
        template<class T> static std::unique_ptr<HypersuccinctTree> create(const std::shared_ptr<UnorderedTree<T>> tree, bool huffman = false, uint32_t sizeMiniParam = 0, uint32_t sizeMicroParam = 0, bool doQueries = true) {
            std::unique_ptr<HypersuccinctTree> hypersuccinctTree = std::unique_ptr<HypersuccinctTree>(new HypersuccinctTree());
            hypersuccinctTree->huffmanFlag = huffman;

            uint32_t sizeMini = sizeMiniParam == 0 ? ceil(pow(log2(tree->getSize()), 2.0)) : sizeMiniParam;
            uint32_t sizeMicro = sizeMicroParam == 0 ? ceil((log2(tree->getSize())) / 8.0) : sizeMicroParam;

            encodeAllSizesInHST(*hypersuccinctTree, tree->getSize(), sizeMini, sizeMicro);

            std::vector<std::shared_ptr<UnorderedTree<T>>> fmMiniTrees = FarzanMunro<T>::decompose(tree, sizeMini);

            std::tie(hypersuccinctTree->miniFIDs, hypersuccinctTree->miniTypeVectors) = create1_2_Interconnections(tree,fmMiniTrees,sizeMini);
            hypersuccinctTree->miniDummys = createDummyInterconnections(tree,fmMiniTrees,sizeMini);

            enumerateMiniTrees(fmMiniTrees);

            PHT_LOGGER_INFO("Factory Create") << "Amount of Minitrees: " << fmMiniTrees.size() << pht::Logger::endl();

            std::map<std::vector<bool>,uint32_t> bpsAndOccurrences;
            createMiniTrees(*hypersuccinctTree, tree, fmMiniTrees, sizeMicro, bpsAndOccurrences,doQueries);

            if(hypersuccinctTree->huffmanFlag) {
                std::map<std::vector<bool>,std::vector<bool>> huffmanTable = Huffman::generateTable<std::vector<bool>>(bpsAndOccurrences);
                convertToHuffman(*hypersuccinctTree, huffmanTable);
            }

            PHT_LOGGER_INFO("Factory Create") << "Finished Creating Hypersuccinct Tree" << pht::Logger::endl();

            hypersuccinctTree->lookupTable.shrink_to_fit();

            //TODO: convert std::bool to BitVector
            createBitvectorSupports(*hypersuccinctTree);

            return hypersuccinctTree;
        }

        /**
         * Creates a HypersuccinctTree from a given full Bitvector
         * WARNING: Function is very sensitive to badly formatted bitvectors (see write to File for formatting)
         * TODO: Need error handling for bad bitvectors
         * @param fullBitvector the bitvector
         * @return Hypersuccinct Tree Class representing the encoded Tree
         */
        static HypersuccinctTree createFromFile(Bitvector& fullBitvector) {
            HypersuccinctTree hst;
            auto iter = fullBitvector.cbegin();
            auto end = fullBitvector.cend();
            hst.huffmanFlag = *iter;
            iter++;
            uint32_t treeSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            uint32_t miniSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            uint32_t microSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            Bitvector_Utils::encodeNumber(hst.size, treeSize, Bitvector_Utils::NumberEncoding::BINARY);
            Bitvector_Utils::encodeNumber(hst.miniSize, miniSize, Bitvector_Utils::NumberEncoding::BINARY);
            Bitvector_Utils::encodeNumber(hst.microSize, microSize, Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t miniTreesSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            uint32_t lookupTableSize = Bitvector_Utils::decodeNumber(iter, fullBitvector.cend(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

            createBitvectorFromFile(iter, end, hst.miniFIDs);
            createBitvectorFromFile(iter, end, hst.miniFIDTopTree);
            createBitvectorFromFile(iter, end, hst.miniFIDLowTree);
            createBitvectorFromFile(iter, end, hst.miniTypeVectors);
            createBitvectorFromFile(iter, end, hst.miniDummys);

            for(uint32_t j=0; j<miniTreesSize; j++) {
                MiniTree mini;
                createBitvectorFromFile(iter, end, mini.FIDs);
                createBitvectorFromFile(iter, end, mini.typeVectors);
                createBitvectorFromFile(iter, end, mini.dummys);
                createBitvectorFromFile(iter, end, mini.microTrees);

                createBitvectorFromFile(iter, end, mini.miniTopFIDIndex);
                createBitvectorFromFile(iter, end, mini.miniLowFIDIndex);
                createBitvectorFromFile(iter, end, mini.microTopFIDIndices);
                createBitvectorFromFile(iter, end, mini.microLowFIDIndices);
                createBitvectorFromFile(iter, end, mini.microFIDTopTrees);
                createBitvectorFromFile(iter, end, mini.microFIDLowTrees);

                createBitvectorFromFile(iter, end, mini.rootAncestors);
                createBitvectorFromFile(iter, end, mini.dummyAncestors);
                createBitvectorFromFile(iter, end, mini.miniDummyTree);
                createBitvectorFromFile(iter, end, mini.miniDummyIndex);
                createBitvectorFromFile(iter, end, mini.miniDummyPointer);
                createBitvectorFromFile(iter, end, mini.microDummyPointers);

                createBitvectorFromFile(iter, end, mini.miniChildRank);
                createBitvectorFromFile(iter, end, mini.microChildRanks);
                createBitvectorFromFile(iter, end, mini.microExtendedChildRanks);

                createBitvectorFromFile(iter, end, mini.miniParent);
                createBitvectorFromFile(iter, end, mini.microParents);

                createBitvectorFromFile(iter, end, mini.subTree);
                createBitvectorFromFile(iter, end, mini.microSubTrees);
                createBitvectorFromFile(iter, end, mini.miniDepth);
                createBitvectorFromFile(iter, end, mini.miniHeight);
                createBitvectorFromFile(iter, end, mini.miniDummyDepth);
                createBitvectorFromFile(iter, end, mini.miniDummyHeight);
                createBitvectorFromFile(iter, end, mini.rootDepths);
                createBitvectorFromFile(iter, end, mini.rootHeights);
                createBitvectorFromFile(iter, end, mini.miniLeaves);
                createBitvectorFromFile(iter, end, mini.microLeaves);
                createBitvectorFromFile(iter, end, mini.miniTreeLeftmostLeafPointer);
                createBitvectorFromFile(iter, end, mini.miniTreeRightmostLeafPointer);
                createBitvectorFromFile(iter, end, mini.microTreeLeftmostLeafPointers);
                createBitvectorFromFile(iter, end, mini.microTreeRightmostLeafPointers);
                createBitvectorFromFile(iter, end, mini.miniRootLeafRank);
                createBitvectorFromFile(iter, end, mini.miniDummyLeafRank);
                createBitvectorFromFile(iter, end, mini.microRootLeafRanks);
                createBitvectorFromFile(iter, end, mini.microExtendedLeafRanks);
                hst.miniTrees.push_back(mini);
            }
            for(uint32_t j=0; j<lookupTableSize; j++) {
                Bitvector index;
                createBitvectorFromFile(iter, end, index);
                Bitvector bp;
                createBitvectorFromFile(iter, end, bp);
                Bitvector ancMatrix;
                createBitvectorFromFile(iter, end, ancMatrix);
                Bitvector childMatrix;
                createBitvectorFromFile(iter, end, childMatrix);
                std::vector<Bitvector> parents;
                createBitvectorFromFile(iter, end, parents);
                std::vector<Bitvector> degree;
                createBitvectorFromFile(iter, end, degree);
                std::vector<Bitvector> subTrees;
                createBitvectorFromFile(iter, end, subTrees);
                std::vector<Bitvector> nodeDepths;
                createBitvectorFromFile(iter, end, nodeDepths);
                std::vector<Bitvector> nodeHeights;
                createBitvectorFromFile(iter, end, nodeHeights);
                std::vector<Bitvector> leaves;
                createBitvectorFromFile(iter, end, leaves);
                std::vector<Bitvector> leftmost_leaf;
                createBitvectorFromFile(iter, end, leftmost_leaf);
                std::vector<Bitvector> rightmost_leaf;
                createBitvectorFromFile(iter, end, rightmost_leaf);
                std::vector<Bitvector> leaf_rank;
                createBitvectorFromFile(iter, end, leaf_rank);

                //TODO: Current Solution - Padding the Bitvector with one full Byte
                /**
                 * On TreeAlex:
                 * 1111111010110000100110001000000010000000100000001 correct bitvector
                 * 1111111010110000100110001000000010000000100000000 createFromFile
                 * 1111111010110000100110001000000010000000100000001 fileoutput
                 * 1111111010110000100110001000000010000000100000000000000 fileinput???
                 */
                LookupTableEntry microTreeData(index, bp);
                microTreeData.ancestorMatrix = ancMatrix;
                microTreeData.childMatrix = childMatrix;
                microTreeData.parentPointers = parents;
                microTreeData.degree = degree;
                microTreeData.subTrees = subTrees;
                microTreeData.nodeDepths = nodeDepths;
                microTreeData.nodeHeights = nodeHeights;
                microTreeData.leaves = leaves;
                microTreeData.leftmost_leaf = leftmost_leaf;
                microTreeData.rightmost_leaf = rightmost_leaf;
                microTreeData.leafRank = leaf_rank;
                hst.lookupTable.push_back(microTreeData);
            }
            createBitvectorSupports(hst);
            return hst;
        }

        /**
         * Reads a Bitvector from a larger Bitvector and writes it into given target
         * Decoding of Larger Bitvector according to FileOutput (See HypersuccinctTreeOutput)
         * @param iter Current Position in large Bitvector
         * @param end End of large Bitvector
         * @param target Bitvector to write into
         */
        static void createBitvectorFromFile(Bitvector::const_iterator& iter, Bitvector::const_iterator& end, Bitvector& target){
            uint32_t tempSize = Bitvector_Utils::decodeNumber(iter, end, Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            target.reserve(tempSize);
            for(uint32_t i=0; i<tempSize; i++) {
                target.emplace_back(*iter);
                iter++;
            }
        }

        /**
         * Reads a Bitvector from a larger Bitvector and writes it into given target
         * Decoding of Larger Bitvector according to FileOutput (See HypersuccinctTreeOutput)
         * @param iter Current Position in large Bitvector
         * @param end End of large Bitvector
         * @param target Bitvector to write into
         */
        static void createBitvectorFromFile(Bitvector::const_iterator& iter, Bitvector::const_iterator& end, std::vector<Bitvector>& target){
            uint32_t tempSize = Bitvector_Utils::decodeNumber(iter, end, Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            for(uint32_t i=0; i<tempSize; i++) {
                Bitvector part;
                createBitvectorFromFile(iter, end, part);
                target.push_back(part);
            }
            target.shrink_to_fit();
        }

        /**
         * This Class creates a Bitvector from a List of given UnorderedTrees
         * It is supposed to use a List of MicroTrees from the Farzan Munro Algorithm
         * Encoding consists of Elias Gamma Code of the size of the MicroTrees and their structure in Balanced Parenthesis form.
         *
         * @tparam T Class implemented in UnorderedTree
         * @param fmMicroTrees List of UnorderedTrees (MicroTrees)
         */
        template<class T> static std::vector<Bitvector> createBitVectorforMicroTrees(std::vector<std::shared_ptr<UnorderedTree<T>>> fmMicroTrees) {
            //determine size of Bitvector
            std::vector<Bitvector> res;
            //Bitvector result;
            for(std::shared_ptr<UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                int32_t size = fmMicroTree->getSize();
                //BP FORM in bitform
                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                res.push_back(bp);
            }
            res.shrink_to_fit();
            return res;
        }

        /**
         * Converts a Hypersuccinct Tree to Huffman encoding
         * Converts MicroTree Encoding from BP-form to huffman codes
         * TODO: Could be private
         *
         * @param tree The Hypersuccinct Tree as HypersuccinctTree
         * @param huffmanTable  The huffman Table containing the ocurrences of all MicroTree structures
         */
        static void convertToHuffman(HypersuccinctTree& tree, std::map<std::vector<bool>,std::vector<bool>> huffmanTable) {
            for(LookupTableEntry& x : tree.lookupTable) {
                x.bp = x.index;
                x.index = huffmanTable.at(x.index);
            }
            for(MiniTree& x : tree.miniTrees) {
                std::vector<std::vector<bool>> oldEncodedMicros = x.microTrees;
                x.microTrees.clear();
                uint32_t entryCount = oldEncodedMicros.size();
                for(uint32_t i = 0; i < entryCount; i++) {
                    std::vector<bool> bp = oldEncodedMicros.at(i);
                    x.microTrees.push_back(huffmanTable.at(bp));
                }
                x.microTrees.shrink_to_fit();
                //HypersuccinctTreeOutput::printBitvector(x.microTrees);
            }
        }

        /**
         * Creates Type 1 and 2 Interconnections between Subtrees of a BaseTree
         * Creates Interconnections according to FarzanMunro
         *
         * @tparam T Class implemented in UnorderedTree
         * @param baseTree Base Tree as UnorderedTree
         * @param subtrees SubTrees as  as Vector of UnorderedTrees
         * @param size Calculated Subtree Size according to FarzanMunro as uint32_t
         * @return Tuple of Bitvectors specifying Interconnections
         */
        template<class T> static std::tuple<std::vector<Bitvector>,std::vector<Bitvector>> create1_2_Interconnections(std::shared_ptr<UnorderedTree<T>> baseTree, std::vector<std::shared_ptr<UnorderedTree<T>>> subtrees, uint32_t size) {
            std::vector<Bitvector> FIDs;
            std::vector<Bitvector> typeVectors;
            uint32_t dummySize = floor(log2(2*size+1))+1;
            std::vector<std::shared_ptr<Node<T>>> rootNodes = ListUtils::mapped<std::shared_ptr<UnorderedTree<T>>, std::shared_ptr<Node<T>>>(subtrees, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getRoot();});
            std::vector<std::shared_ptr<Node<T>>> distinctRootNodes = ListUtils::distincted(rootNodes);
            std::vector<std::shared_ptr<Node<T>>> firstChildren;
            std::vector<std::shared_ptr<UnorderedTree<T>>> filteredTrees = ListUtils::filtered<std::shared_ptr<UnorderedTree<T>>>(subtrees, [](std::shared_ptr<UnorderedTree<T>> x){return !(x -> isLeaf(x->getRoot()));});
            firstChildren = ListUtils::mapped<std::shared_ptr<UnorderedTree<T>>, std::shared_ptr<Node<T>>>(filteredTrees, [](std::shared_ptr<UnorderedTree<T>> x){return x -> getDirectDescendants(x->getRoot()).at(0);});
            //zählung von firstChildren ist front - zählung in enumerate

            //FIDs und TypeVectors
            for(std::shared_ptr<Node<T>> rootNode : distinctRootNodes) {
                std::vector<std::shared_ptr<Node<T>>> children = baseTree->getDirectDescendants(rootNode);
                Bitvector fidPart;
                Bitvector tvPart;
                fidPart.reserve(children.size());
                for(std::shared_ptr<Node<T>> node : children) {
                    if(ListUtils::containsAny(rootNodes,{node})) {
                        fidPart.emplace_back(true);
                        tvPart.push_back(false);
                    }
                    else if(ListUtils::containsAny(firstChildren,{node}))
                    {
                        fidPart.emplace_back(true);
                        tvPart.push_back(true);
                    }
                    else
                    {
                        fidPart.emplace_back(false);
                    }
                }
                FIDs.push_back(fidPart);
                typeVectors.push_back(tvPart);

            }
            FIDs.shrink_to_fit();
            typeVectors.shrink_to_fit();
            return {FIDs, typeVectors};
        }

        static void encodeAllSizesInHST(HypersuccinctTree &hypersuccinctTree, uint32_t sizeTree, uint32_t sizeMini, uint32_t sizeMicro){
            Bitvector_Utils::encodeNumber(std::inserter(hypersuccinctTree.size, hypersuccinctTree.size.begin()), sizeTree, Bitvector_Utils::NumberEncoding::BINARY);
            Bitvector_Utils::encodeNumber(std::inserter(hypersuccinctTree.miniSize, hypersuccinctTree.miniSize.begin()), sizeMini,Bitvector_Utils::NumberEncoding::BINARY);
            Bitvector_Utils::encodeNumber(std::inserter(hypersuccinctTree.microSize, hypersuccinctTree.microSize.begin()), sizeMicro,Bitvector_Utils::NumberEncoding::BINARY);
        }

        /**
         * Creates Dummy Interconnections between Subtrees of a BaseTree
         * Creates Interconnections according to FarzanMunro
         *
         * @tparam T Class implemented in UnorderedTree
         * @param baseTree Base Tree as UnorderedTree
         * @param subtrees SubTrees as  as Vector of UnorderedTrees
         * @param size Calculated Subtree Size according to FarzanMunro as uint32_t
         * @return Bitvector specifying the Dummy Interconections
         */
        template<class T> static std::vector<Bitvector> createDummyInterconnections(std::shared_ptr<UnorderedTree<T>> baseTree, std::vector<std::shared_ptr<UnorderedTree<T>>> subtrees, uint32_t size) {
            std::vector<Bitvector> dummys;
            uint32_t dummySize = floor(log2(2*size+1))+1;
            //Dummy Nodes
            for(std::shared_ptr<UnorderedTree<T>> fmMicroTree : subtrees) {
                bool hadDummy = false;
                for(std::shared_ptr<Node<T>> node : fmMicroTree->getNodes()) {
                    if(node != fmMicroTree->getRoot()) {
                        std::vector<std::shared_ptr<Node<T>>> children = baseTree->getDirectDescendants(node);
                        for(int ind = 0; ind < children.size(); ind++) {
                            if(!ListUtils::containsAny(fmMicroTree->getNodes(),{children.at(ind)})) {
                                std::shared_ptr<Node<T>> dummyNode = std::make_shared<Node<T>>(T());
                                dummyNode->setMiniDummy();
                                //Index für Tree order
                                fmMicroTree->insert(dummyNode, ind, node);
                                fmMicroTree->setDummy(dummyNode);
                                baseTree->insertBetween(dummyNode, children.at(ind), node);
                                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                                Bitvector num;
                                uint32_t enumV = fmMicroTree->enumerate(dummyNode);
                                Bitvector_Utils::encodeNumber(std::inserter(num, num.end()), enumV ,Bitvector_Utils::NumberEncoding::BINARY);
                                dummys.push_back(num);
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
                    dummys.push_back({false});
                }
            }
            dummys.shrink_to_fit();
            return dummys;
        }

        /**
         * Fills the given Lookup Table entry with data from the given MicroTree
         * @tparam T Class implemented in UnorderedTree
         * @param lookupTableEntry The lookupTable Entry to fill
         * @param fmMicroTree The Micro Tree to get data from
         */
        template<class T> static void fillLookupTableEntry(LookupTableEntry& lookupTableEntry, const std::shared_ptr<UnorderedTree<T>>& fmMicroTree){
            PHT_LOGGER_INFO("Factory Create") << "Creating LookupTableEntries..." << pht::Logger::endl();
            std::vector<std::shared_ptr<Node<T>>> nodes = fmMicroTree->getNodes();
            //Generates LookupTable Entries
            for(std::shared_ptr<Node<T>> node1 : fmMicroTree->getNodes()) {

                std::vector<std::shared_ptr<pht::Node<T>>> directDesc = fmMicroTree->getDirectDescendants(node1);
                for(std::shared_ptr<Node<T>> node2 : fmMicroTree->getNodes()) {
                    lookupTableEntry.ancestorMatrix.push_back(fmMicroTree->isAncestor(node2, node1));
                    lookupTableEntry.childMatrix.push_back(ListUtils::containsAny(directDesc, {node2}));
                }


                if(fmMicroTree->isRoot(node1)) {
                    lookupTableEntry.parentPointers.push_back({false});
                } else {
                    auto iter1 = std::find(nodes.begin(), nodes.end(), fmMicroTree->getDirectAncestor(node1));
                    uint32_t parentNum = std::distance(nodes.begin(), iter1) + 1;
                    lookupTableEntry.parentPointers.push_back(Bitvector_Utils::encodeNumberReturn(parentNum));
                }

                uint32_t degreeNum = fmMicroTree->getDegree(node1) + 1;
                lookupTableEntry.degree.push_back(Bitvector_Utils::encodeNumberReturn(degreeNum));

                uint32_t subTreeNum = fmMicroTree->getSize(node1,true) + 1;
                lookupTableEntry.subTrees.push_back(Bitvector_Utils::encodeNumberReturn(subTreeNum));

                uint32_t nodeDepthNum = fmMicroTree->getDepth(node1, true) + 1;
                lookupTableEntry.nodeDepths.push_back(Bitvector_Utils::encodeNumberReturn(nodeDepthNum));

                uint32_t nodeHeightNum = fmMicroTree->getHeight(node1, true) + 1;
                lookupTableEntry.nodeHeights.push_back(Bitvector_Utils::encodeNumberReturn(nodeHeightNum));

                uint32_t leaveNum = fmMicroTree->getLeafSize(node1);
                lookupTableEntry.leaves.push_back(Bitvector_Utils::encodeNumberReturn(leaveNum));

                uint32_t leftmost_leafNum =  std::distance(nodes.begin(), std::find(nodes.begin(),nodes.end(),fmMicroTree->getLeftmostLeaf(node1)));
                lookupTableEntry.leftmost_leaf.push_back(Bitvector_Utils::encodeNumberReturn(leftmost_leafNum));

                uint32_t rightmost_leafNum = std::distance(nodes.begin(), std::find(nodes.begin(),nodes.end(),fmMicroTree->getRightmostLeaf(node1)));
                lookupTableEntry.rightmost_leaf.push_back(Bitvector_Utils::encodeNumberReturn(rightmost_leafNum));

                uint32_t leafRankNum = fmMicroTree->getLeafRank(node1) + 1;
                lookupTableEntry.leafRank.push_back(Bitvector_Utils::encodeNumberReturn(leafRankNum));
            }
            lookupTableEntry.parentPointers.shrink_to_fit();
            lookupTableEntry.leafRank.shrink_to_fit();
            lookupTableEntry.degree.shrink_to_fit();
            lookupTableEntry.nodeHeights.shrink_to_fit();
            lookupTableEntry.rightmost_leaf.shrink_to_fit();
            lookupTableEntry.leftmost_leaf.shrink_to_fit();
            lookupTableEntry.nodeDepths.shrink_to_fit();
            lookupTableEntry.subTrees.shrink_to_fit();
            lookupTableEntry.leaves.shrink_to_fit();
            PHT_LOGGER_INFO("Factory Create") << "Finished creating LookupTableEntries." << pht::Logger::endl();
        }

        /**
         * Uses the MicroTrees for a given Mini Tree and fills the HypersuccinctTree and MiniTree with data.
         * @tparam T Class implemented in UnorderedTree
         * @param hypersuccinctTree The Hypersuccinct Tree to fill
         * @param miniTree The MiniTree to fill
         * @param fmMiniTree The Mini Tree to get data from
         * @param fmMicroTrees The Micro Trees to get data from
         * @param bpsAndOccurrences Counting Table of BP forms for Huffman encoding
         */
        template<class T> static void createMicroTrees(HypersuccinctTree& hypersuccinctTree, const std::shared_ptr<UnorderedTree<T>>& tree, MiniTree& miniTree, std::shared_ptr<UnorderedTree<T>>& fmMiniTree, std::vector<std::shared_ptr<UnorderedTree<T>>>& fmMicroTrees, std::map<std::vector<bool>,uint32_t>& bpsAndOccurrences,uint32_t sizeMicro, bool doQueries,std::vector<std::mutex>& allMutex){
            std::unique_lock<std::mutex> lockLog(allMutex.at(2));
            PHT_LOGGER_INFO("Factory Create") << "Creating MicroTrees for a MiniTree..." << pht::Logger::endl();
            lockLog.unlock();
            uint32_t microCount = 0;
            //The actual MicroTree Loop
            //Put everything that needs MicroTree Iteration in this loop
            for(const std::shared_ptr<UnorderedTree<T>>& fmMicroTree : fmMicroTrees) {

                handleMiniDummyInMicroTree(miniTree, fmMiniTree, fmMicroTree, fmMicroTrees);

                //MicroDummyPointers
                uint32_t dummySize = floor(log2(2*sizeMicro+1))+1;
                if(fmMicroTree->hasDummy()) {
                    std::shared_ptr<Node<T>> dummyPoint = fmMiniTree->getDirectDescendants(fmMicroTree->getDummy()).at(0);
                    uint32_t microTreePointer = dummyPoint->getMicroTree();
                    miniTree.microDummyPointers.push_back(Bitvector_Utils::encodeNumberReturn(microTreePointer));
                } else {
                    miniTree.microDummyPointers.push_back({false});
                }

                if(doQueries) {
                    //Dummy Ancestory
                    if (fmMiniTree->hasDummy()) {
                        if (fmMicroTree->hasDummy()) {
                            miniTree.dummyAncestors.push_back(
                                    fmMiniTree->isAncestor(fmMiniTree->getDummy(), fmMicroTree->getDummy()));
                        } else {
                            miniTree.dummyAncestors.push_back(false);
                        }
                        miniTree.rootAncestors.push_back(
                                fmMiniTree->isAncestor(fmMiniTree->getDummy(), fmMicroTree->getRoot()));
                    }

                    //Additions for Queries - MicroTrees
                    std::shared_ptr<Node<T>> microRoot = fmMicroTree->getRoot();
                    miniTree.microSubTrees.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getSize(microRoot, false)));
                    miniTree.rootDepths.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getDepth(microRoot) + 1));
                    miniTree.rootHeights.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getHeight(microRoot) + 1));
                    miniTree.microLeaves.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getLeafSize(microRoot)));
                    miniTree.microTreeLeftmostLeafPointers.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getLeftmostLeaf(microRoot)->getMicroTree()));
                    miniTree.microTreeRightmostLeafPointers.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getRightmostLeaf(microRoot)->getMicroTree()));

                    miniTree.microTopFIDIndices.push_back({false});
                    miniTree.microLowFIDIndices.push_back({false});

                    if (fmMiniTree->isRoot(microRoot)) {
                        miniTree.microParents.push_back({false});
                    } else {
                        miniTree.microParents.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getDirectAncestor(microRoot)->getMicroTree() + 1));
                    }

                    if (fmMiniTree->isRoot(microRoot)) {

                        std::vector<std::shared_ptr<Node<T>>> children = fmMiniTree->getDirectDescendants(microRoot);
                        if (children.empty()) {
                            miniTree.microRootLeafRanks.push_back({false});
                            miniTree.microChildRanks.push_back({false});
                        } else {
                            miniTree.microRootLeafRanks.push_back(Bitvector_Utils::encodeNumberReturn(tree->getLeafRank(children.at(0)) + 1));
                            miniTree.microChildRanks.push_back(Bitvector_Utils::encodeNumberReturn(tree->getChildRank(children.at(0)) + 1));
                        }
                    } else {
                        miniTree.microRootLeafRanks.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getLeafRank(microRoot) + 1));
                        miniTree.microChildRanks.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getChildRank(microRoot) + 1));
                    }
                    std::vector<std::shared_ptr<Node<T>>> microChildren = fmMicroTree->getDirectDescendants(microRoot);
                    if (microChildren.empty()) {
                        miniTree.microExtendedLeafRanks.push_back({true});
                        miniTree.microExtendedChildRanks.push_back({false});
                    } else {
                        bool removeLeafMan = false;
                        if (fmMiniTree->isLeaf(microChildren.at(0))) {
                            removeLeafMan = true;
                        }
                        miniTree.microExtendedLeafRanks.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getLeafRank(microChildren.at(0)) + 1 - removeLeafMan));
                        miniTree.microExtendedChildRanks.push_back(Bitvector_Utils::encodeNumberReturn(fmMiniTree->getChildRank(microChildren.at(0)) + 1));
                    }
                }

                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                if(hypersuccinctTree.huffmanFlag) {
                    if(bpsAndOccurrences.find(bp) == bpsAndOccurrences.end()) {
                        bpsAndOccurrences.insert({bp, 0});
                    }
                    bpsAndOccurrences.at(bp)++;
                }

                LookupTableEntry microTreeData(bp);
                std::unique_lock<std::mutex> lockLookup(allMutex.at(1));
                if(!ListUtils::containsAny(hypersuccinctTree.lookupTable, {microTreeData})) {
                    if(doQueries) {
                        fillLookupTableEntry(microTreeData, fmMicroTree);
                    }
                    hypersuccinctTree.lookupTable.push_back(microTreeData);
                }
                lockLookup.unlock();
            }
            if(doQueries) {
                std::vector<std::pair<std::vector<uint32_t>, std::vector<uint32_t>>> fIDTreesVector = getTreesForMicroFID(hypersuccinctTree, miniTree);
                for (uint32_t i=0; i < fIDTreesVector.size();i++) {
                    std::pair<std::vector<uint32_t>, std::vector<uint32_t>> fIDTrees = fIDTreesVector.at(i);
                    if (fIDTrees.first.empty()) {
                        miniTree.microFIDTopTrees.push_back({false});
                    } else {
                        Bitvector topFirst;
                        Bitvector_Utils::encodeNumber(topFirst, fIDTrees.first.at(0) + 1,Bitvector_Utils::NumberEncoding::BINARY);
                        miniTree.microFIDTopTrees.push_back(topFirst);
                    }
                    if (fIDTrees.second.empty()) {
                        miniTree.microFIDLowTrees.push_back({false});
                    } else {
                        Bitvector lowFirst;
                        Bitvector_Utils::encodeNumber(lowFirst, fIDTrees.second.at(0) + 1,Bitvector_Utils::NumberEncoding::BINARY);
                        miniTree.microFIDLowTrees.push_back(lowFirst);
                    }

                    for(uint32_t topTree : fIDTrees.first) {
                        miniTree.microTopFIDIndices.at(topTree) = (Bitvector_Utils::encodeNumberReturn(i + 1));
                    }
                    for(uint32_t lowTree : fIDTrees.second) {
                        miniTree.microLowFIDIndices.at(lowTree) = (Bitvector_Utils::encodeNumberReturn(i + 1));
                    }
                }
            }
            miniTree.FIDs.shrink_to_fit();
            miniTree.typeVectors.shrink_to_fit();
            miniTree.dummys.shrink_to_fit();
            miniTree.microTopFIDIndices.shrink_to_fit();
            miniTree.microLowFIDIndices.shrink_to_fit();
            miniTree.microFIDTopTrees.shrink_to_fit();
            miniTree.microFIDLowTrees.shrink_to_fit();
            miniTree.microDummyPointers.shrink_to_fit();
            miniTree.microChildRanks.shrink_to_fit();
            miniTree.microExtendedChildRanks.shrink_to_fit();
            miniTree.microParents.shrink_to_fit();
            miniTree.microSubTrees.shrink_to_fit();
            miniTree.rootDepths.shrink_to_fit();
            miniTree.rootHeights.shrink_to_fit();
            miniTree.microLeaves.shrink_to_fit();
            miniTree.microTreeLeftmostLeafPointers.shrink_to_fit();
            miniTree.microTreeRightmostLeafPointers.shrink_to_fit();
            miniTree.microRootLeafRanks.shrink_to_fit();
            miniTree.microExtendedLeafRanks.shrink_to_fit();
            std::unique_lock<std::mutex> lockLog2(allMutex.at(2));
            PHT_LOGGER_INFO("Factory Create", string("Finished Creating MicroTrees for this MiniTree."));
            lockLog2.unlock();
        }

        template<class T> static void createMiniTree(HypersuccinctTree& hypersuccinctTree, const std::shared_ptr<UnorderedTree<T>>& tree, std::shared_ptr<UnorderedTree<T>>& fmMiniTree, uint32_t sizeMicro,std::map<std::vector<bool>,uint32_t>& bpsAndOccurrences, bool doQueries,std::vector<std::mutex>& allMutex,uint32_t miniTreePos) {
            std::vector<std::shared_ptr<UnorderedTree<T>>> fmMicroTrees = FarzanMunro<T>::decomposeMultiThread(fmMiniTree, sizeMicro, allMutex);
            MiniTree miniTree = MiniTree();

            //Creating Micro Interconnections and Dummys
            std::tie(miniTree.FIDs, miniTree.typeVectors) = create1_2_Interconnections(fmMiniTree,fmMicroTrees,sizeMicro);
            std::vector<Bitvector> dummys = createDummyInterconnections(fmMiniTree, fmMicroTrees, sizeMicro);
            miniTree.dummys = dummys;

            enumerateMicroTrees(fmMicroTrees);

            //Simple Additions for Queries - MiniTree
            if(doQueries) {
                std::shared_ptr<Node<T>> miniRoot = fmMiniTree->getRoot();
                Bitvector_Utils::encodeNumber(miniTree.subTree, tree->getSize(miniRoot, false),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniDepth, tree->getDepth(miniRoot),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniHeight, tree->getHeight(miniRoot),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniLeaves, tree->getLeafSize(miniRoot),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniTreeLeftmostLeafPointer,tree->getLeftmostLeaf(miniRoot)->getMiniTree(),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniTreeRightmostLeafPointer,tree->getRightmostLeaf(miniRoot)->getMiniTree(),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniRootLeafRank, tree->getLeafRank(miniRoot),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniChildRank, tree->getChildRank(miniRoot),Bitvector_Utils::NumberEncoding::BINARY);

                miniTree.miniTopFIDIndex = {false};
                miniTree.miniLowFIDIndex = {false};

                if (tree->isRoot(miniRoot)) {
                    miniTree.miniParent = {false};
                } else {
                    Bitvector_Utils::encodeNumber(miniTree.miniParent,tree->getDirectAncestor(miniRoot)->getMiniTree() + 1,Bitvector_Utils::NumberEncoding::BINARY);
                }
            }


            //MiniDummy Bitvectors
            if(fmMiniTree->hasDummy()) {
                std::shared_ptr<Node<T>> dummyPoint = tree->getDirectDescendants(fmMiniTree->getDummy()).at(0);
                uint32_t miniTreePointer = dummyPoint->getMiniTree();
                Bitvector_Utils::encodeNumber(miniTree.miniDummyPointer, miniTreePointer, Bitvector_Utils::NumberEncoding::BINARY);
                if(doQueries) {
                    Bitvector_Utils::encodeNumber(miniTree.miniDummyDepth, tree->getDepth(dummyPoint),Bitvector_Utils::NumberEncoding::BINARY);
                    Bitvector_Utils::encodeNumber(miniTree.miniDummyHeight, tree->getHeight(dummyPoint),Bitvector_Utils::NumberEncoding::BINARY);
                    Bitvector_Utils::encodeNumber(miniTree.miniDummyLeafRank, tree->getLeafRank(dummyPoint),Bitvector_Utils::NumberEncoding::BINARY);
                }
            }

            createMicroTrees(hypersuccinctTree, tree, miniTree, fmMiniTree, fmMicroTrees, bpsAndOccurrences,sizeMicro,doQueries,allMutex);

            //This is done so late because of Huffman checks
            miniTree.microTrees = createBitVectorforMicroTrees(fmMicroTrees);
            miniTree.microTrees.shrink_to_fit();
            std::unique_lock<std::mutex> lockMini(allMutex.at(3));
            hypersuccinctTree.miniTrees.at(miniTreePos) = miniTree;
            lockMini.unlock();

            //Output
            std::unique_lock<std::mutex> lockLog(allMutex.at(2));
            PHT_LOGGER_INFO("FACTORY") << "Size of MiniTree: " << fmMiniTree->getSize() << pht::Logger::endl();
            PHT_LOGGER_INFO("FACTORY") << "Root of MiniTree: " << fmMiniTree->getRoot()->getValue() << pht::Logger::endl();
            PHT_LOGGER_INFO("FACTORY") << "Nodes of MiniTree: " << fmMiniTree->toNewickString() << pht::Logger::endl();
            PHT_LOGGER_INFO("FACTORY") << "Amount of MicroTrees: " << fmMicroTrees.size() << pht::Logger::endl();
            for(std::shared_ptr<UnorderedTree<std::string>>& fmMicroTree : fmMicroTrees) {
                PHT_LOGGER_INFO("FACTORY") << "Size of MicroTree: " << fmMicroTree->getSize() << pht::Logger::endl();
                PHT_LOGGER_INFO("FACTORY") << "Root of MicroTree: " << fmMicroTree->getRoot()->getValue() << pht::Logger::endl();
                PHT_LOGGER_INFO("FACTORY") << "Nodes of MicroTree: " << fmMicroTree->toNewickString() << pht::Logger::endl();
            }
            lockLog.unlock();
        }

        /**
         * Uses the given Mini Trees to fill the HypersuccinctTree  with data
         * @tparam T Class implemented in UnorderedTree
         * @param hypersuccinctTree The Hypersuccinct Tree to fill
         * @param tree The original Tree to get data from
         * @param fmMiniTrees The Mini Trees to get data from
         * @param sizeMicro Average Size of MicroTrees (according to Farzan Munro) for MicroTree generation
         * @param bpsAndOccurrences Count Table of BP forms for Huffman encoding
         */
        template<class T> static void createMiniTrees(HypersuccinctTree& hypersuccinctTree, const std::shared_ptr<UnorderedTree<T>>& tree, std::vector<std::shared_ptr<UnorderedTree<T>>>& fmMiniTrees, uint32_t sizeMicro, std::map<std::vector<bool>,uint32_t>& bpsAndOccurrences, bool doQueries){

            PHT_LOGGER_INFO("Factory Create", string("Creating MiniTrees..."));
            hypersuccinctTree.miniTrees = std::vector<MiniTree>(fmMiniTrees.size());
            /*std::vector<std::thread> allThreads;
            allThreads.reserve(fmMiniTrees.size());
            std::vector<std::mutex> allMutex(4);
            for(uint32_t i = 0; i < fmMiniTrees.size(); i++) {
                std::shared_ptr<UnorderedTree<T>>& fmMiniTree = fmMiniTrees.at(i);
                allThreads.emplace_back(std::thread(createMiniTree<T>,std::ref(hypersuccinctTree),std::cref(tree),std::ref(fmMiniTree),sizeMicro,std::ref(bpsAndOccurrences),doQueries,std::ref(allMutex),i));
            }

            for(std::thread& t : allThreads) {
                t.join();
            }*/
            thread_pool pool;
            std::vector<std::mutex> allMutex(4);
            for(uint32_t i = 0; i < fmMiniTrees.size(); i++) {
                std::shared_ptr<UnorderedTree<T>>& fmMiniTree = fmMiniTrees.at(i);
                pool.push_task(createMiniTree<T>,std::ref(hypersuccinctTree),std::cref(tree),std::ref(fmMiniTree),sizeMicro,std::ref(bpsAndOccurrences),doQueries,std::ref(allMutex),i);
            }
            pool.wait_for_tasks();

            std::cout << "All THREADS DONE" << std::endl;

            if(doQueries) {
                uint32_t miniSize = hypersuccinctTree.miniTrees.size();
                std::vector<std::pair<std::vector<uint32_t>, std::vector<uint32_t>>> fIDTreeVector = getTreesForFID(hypersuccinctTree);
                for (uint32_t i = 0; i < fIDTreeVector.size();i++) {
                    std::pair<std::vector<uint32_t>, std::vector<uint32_t>> fIDTrees = fIDTreeVector.at(i);
                    if (fIDTrees.first.empty()) {
                        hypersuccinctTree.miniFIDTopTree.push_back({false});
                    } else {
                        hypersuccinctTree.miniFIDTopTree.push_back(Bitvector_Utils::encodeNumberReturn(fIDTrees.first.at(0) + 1));
                    }
                    if (fIDTrees.second.empty()) {
                        hypersuccinctTree.miniFIDLowTree.push_back({false});
                    } else {
                        hypersuccinctTree.miniFIDLowTree.push_back(Bitvector_Utils::encodeNumberReturn(fIDTrees.second.at(0) + 1));
                    }
                    for(uint32_t topTree : fIDTrees.first) {
                        MiniTree& miniTree = hypersuccinctTree.getMiniTree(topTree);
                        miniTree.miniTopFIDIndex = Bitvector_Utils::encodeNumberReturn(i + 1);
                    }
                    for(uint32_t lowTree : fIDTrees.second) {
                        MiniTree& miniTree = hypersuccinctTree.getMiniTree(lowTree);
                        miniTree.miniLowFIDIndex = Bitvector_Utils::encodeNumberReturn(i + 1);
                    }
                }
            }
            hypersuccinctTree.miniFIDs.shrink_to_fit();
            hypersuccinctTree.miniFIDTopTree.shrink_to_fit();
            hypersuccinctTree.miniFIDLowTree.shrink_to_fit();
            hypersuccinctTree.miniTypeVectors.shrink_to_fit();
            hypersuccinctTree.miniDummys.shrink_to_fit();
            PHT_LOGGER_INFO("Factory Create") << "Finished Creating MiniTrees." << pht::Logger::endl();
        }

        /**
         * For each Node in the original Tree: Marks which MiniTree it belongs to.
         * @tparam T Class implemented in Unordered Tree
         * @param fmMiniTrees Mini Trees for marking
         */
        template<class T> static void enumerateMiniTrees(std::vector<std::shared_ptr<UnorderedTree<T>>>& fmMiniTrees){
            uint32_t miniTreeNum = 0;
            for(std::shared_ptr<UnorderedTree<T>> fmMiniTree : fmMiniTrees) {
                for(const std::shared_ptr<Node<T>>& node : fmMiniTree->getNodes()) {
                    node->setMiniTree(miniTreeNum);
                }
                miniTreeNum++;
            }
        }

        /**
         * For each Node in the original Tree: Marks which MicroTree it belongs to.
         * @tparam T Class implemented in Unordered Tree
         * @param fmMicroTrees Micro Trees for marking
         */
        template<class T> static void enumerateMicroTrees(std::vector<std::shared_ptr<UnorderedTree<T>>>& fmMicroTrees){
            uint32_t microTreeNum = 0;
            for(const std::shared_ptr<UnorderedTree<T>>& fmMicroTree : fmMicroTrees) {
                for(const std::shared_ptr<Node<T>>& node : fmMicroTree->getNodes()) {
                    node->setMicroTree(microTreeNum);
                }
                microTreeNum++;
            }
        }

        /**
         * Generates Pointers for Mini Dummy so that it can be identified as HSTNode later.
         * @tparam T Class implemented in UnorderedTree
         * @param miniTree The MiniTree containing the Dummy as Minitree
         * @param fmMiniTree The Mini Tree containting the Dummy as UnorderedTree
         * @param fmMicroTree The MicroTree containing the Dummy as UnorderedTree
         * @param fmMicroTrees Vector of all MicroTrees as UnorderedTree
         */
        template<class T> static void handleMiniDummyInMicroTree(MiniTree& miniTree, std::shared_ptr<UnorderedTree<T>>& fmMiniTree, const std::shared_ptr<UnorderedTree<T>>& fmMicroTree, std::vector<std::shared_ptr<UnorderedTree<T>>>& fmMicroTrees){
            if(fmMicroTree->contains(fmMiniTree->getDummy())) {
                auto iter = std::find(fmMicroTrees.begin(),fmMicroTrees.end(), fmMicroTree);
                uint32_t dist = std::distance(fmMicroTrees.begin(), iter);
                Bitvector_Utils::encodeNumber(miniTree.miniDummyTree,dist,Bitvector_Utils::NumberEncoding::BINARY);
                uint32_t enumV = fmMicroTree->enumerate(fmMiniTree->getDummy());
                Bitvector_Utils::encodeNumber(miniTree.miniDummyIndex,enumV,Bitvector_Utils::NumberEncoding::BINARY);
            }
        }

        static std::vector< std::pair< std::vector<uint32_t >,std::vector<uint32_t > > > getTreesForFID(const HypersuccinctTree& hypersuccinctTree) {

            std::vector<uint32_t > topTree;
            topTree.push_back(0);
            uint32_t topOffset = 0;
            uint32_t lowOffset = 0;
            uint32_t currentIndex = 0;

            std::vector<std::pair<std::vector<uint32_t >,std::vector<uint32_t >>> result;
            result.reserve(hypersuccinctTree.miniFIDs.size());

            while(currentIndex < hypersuccinctTree.miniFIDs.size()) {
                Bitvector fid = hypersuccinctTree.miniFIDs.at(currentIndex);
                uint32_t topTrees = Bitvector_Utils::countOccurences(hypersuccinctTree.miniTypeVectors.at(currentIndex).cbegin(), hypersuccinctTree.miniTypeVectors.at(currentIndex).cend());
                uint32_t lowTrees = Bitvector_Utils::countOccurences(hypersuccinctTree.miniTypeVectors.at(currentIndex).cbegin(), hypersuccinctTree.miniTypeVectors.at(currentIndex).cend(),true);

                if(topTrees == 0) {
                    topTrees = 1;
                }
                if(currentIndex==0) {
                    lowOffset = topTrees;
                }


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
                result.emplace_back(topTreeIndices,lowTreeIndices);


                for(int i=1; i<= topTrees; i++) {
                    if(topTree.size()<=topOffset+i) {
                        topTree.push_back(topTree.at(topOffset) + topTrees);
                    }
                }
                topOffset += topTrees;
                lowOffset += lowTrees;

                currentIndex++;
            }
            return result;
        }

        static std::vector< std::pair<std::vector<uint32_t>,std::vector<uint32_t>>> getTreesForMicroFID(const HypersuccinctTree& hypersuccinctTree, MiniTree &miniTree) {

            std::vector<uint32_t > topTree;
            topTree.push_back(0);
            uint32_t topOffset = 0;
            uint32_t lowOffset = 0;
            uint32_t currentIndex = 0;

            std::vector<std::pair<std::vector<uint32_t >,std::vector<uint32_t >>> result;
            result.reserve(miniTree.FIDs.size());

            while(currentIndex < miniTree.FIDs.size()) {
                Bitvector fid = miniTree.FIDs.at(currentIndex);
                uint32_t topTrees = Bitvector_Utils::countOccurences(miniTree.typeVectors.at(currentIndex).cbegin(), miniTree.typeVectors.at(currentIndex).cend());
                uint32_t lowTrees = Bitvector_Utils::countOccurences(miniTree.typeVectors.at(currentIndex).cbegin(), miniTree.typeVectors.at(currentIndex).cend(),true);

                if(topTrees == 0) {
                    topTrees = 1;
                }
                if(currentIndex==0) {
                    lowOffset = topTrees;
                }


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
                result.emplace_back(topTreeIndices,lowTreeIndices);


                for(int i=1; i<= topTrees; i++) {
                    if(topTree.size()<=topOffset+i) {
                        topTree.push_back(topTree.at(topOffset) + topTrees);
                    }
                }
                topOffset += topTrees;

                lowOffset += lowTrees;

                currentIndex++;
            }
            return result;
        }

        static void createBitvectorSupports(HypersuccinctTree& hst) {
            thread_pool pool;
            pool.push_task(assignBitVector,std::ref(hst.miniFIDsSupport),std::cref(hst.miniFIDs));
            pool.push_task(assignBitVector,std::ref(hst.miniTypeVectorsSupport),std::cref(hst.miniTypeVectors));
            //assignBitVector(hst.miniFIDsSupport , hst.miniFIDs);
            //assignBitVector(hst.miniTypeVectorsSupport , hst.miniTypeVectors);
            /*std::vector<std::mutex> allMutex(4);
            for(uint32_t i = 0; i < fmMiniTrees.size(); i++) {
                std::shared_ptr<UnorderedTree<T>>& fmMiniTree = fmMiniTrees.at(i);
                pool.push_task(createMiniTree<T>,std::ref(hypersuccinctTree),std::cref(tree),std::ref(fmMiniTree),sizeMicro,std::ref(bpsAndOccurrences),doQueries,std::ref(allMutex),i);
            }
            pool.wait_for_tasks();*/
            for(MiniTree &miniTree: hst.getMiniTrees()) {
                pool.push_task(assignBitVector,std::ref(miniTree.FIDsSupport),std::cref(miniTree.FIDs));
                pool.push_task(assignBitVector,std::ref(miniTree.typeVectorsSupport),std::cref(miniTree.typeVectors));
                //assignBitVector(miniTree.FIDsSupport , miniTree.FIDs);
                //assignBitVector(miniTree.typeVectorsSupport , miniTree.typeVectors);
            }
            for(LookupTableEntry &entry : hst.lookupTable) {
                pool.push_task(assignBitvector,std::ref(entry.ancestorMatrixSupport),std::cref(entry.ancestorMatrix));
                pool.push_task(assignBitvector,std::ref(entry.childMatrixSupport),std::cref(entry.childMatrix));
                //assignBitvector(entry.ancestorMatrixSupport , entry.ancestorMatrix);
                //assignBitvector(entry.childMatrixSupport , entry.childMatrix);
            }
            pool.wait_for_tasks();
        }

        static void assignBitvector(succinct_bv::BitVector& bitVector, const Bitvector& bitvector) {
            if(!bitvector.empty()) {
                bitVector = bitvector;
            }
        }

        static void assignBitVector(std::vector<succinct_bv::BitVector>& bitVector, const std::vector<Bitvector>& bitvector) {
            if(!bitvector.empty()) {
                bitVector.reserve(bitvector.size());
                for (const Bitvector &bit : bitvector) {
                    if(bit.empty()) {
                        bitVector.emplace_back(succinct_bv::BitVector());
                    } else {
                        bitVector.emplace_back(succinct_bv::BitVector(bit));
                    }
                }
            }
        }
    };
}

#undef DLL_API
#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H