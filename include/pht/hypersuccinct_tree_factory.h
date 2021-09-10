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

#include "logger.h"
#include "huffman.h"
#include "hypersuccinct_tree.h"
#include "hst_output.h"

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
        template<class T> static std::unique_ptr<HypersuccinctTree> create(const std::shared_ptr<UnorderedTree<T>> tree, bool huffman = false, uint32_t sizeMiniParam = 0, uint32_t sizeMicroParam = 0) {
            std::unique_ptr<HypersuccinctTree> hypersuccinctTree = std::unique_ptr<HypersuccinctTree>(new HypersuccinctTree());
            hypersuccinctTree->huffmanFlag = huffman;

            uint32_t sizeMini = sizeMiniParam == 0 ? ceil(pow(log2(tree->getSize()), 2.0)) : sizeMiniParam;
            uint32_t sizeMicro = sizeMicroParam == 0 ? ceil((log2(tree->getSize())) / 8.0) : sizeMicroParam;

            encodeAllSizesInHST(*hypersuccinctTree, tree->getSize(), sizeMini, sizeMicro);

            std::vector<std::shared_ptr<UnorderedTree<T>>> fmMiniTrees = FarzanMunro<T>::decompose(tree, sizeMini);

            std::tie(hypersuccinctTree->miniFIDs, hypersuccinctTree->miniTypeVectors) = create1_2_Interconnections(tree,fmMiniTrees,sizeMini);
            hypersuccinctTree->miniDummys = createDummyInterconnections(tree,fmMiniTrees,sizeMini);

            enumerateMiniTrees(fmMiniTrees);

            PHT_LOGGER_INFO("Factory Create", string ("Amount of Minitrees: ") + to_string(fmMiniTrees.size()));

            std::map<std::vector<bool>,uint32_t> bpsAndOccurrences;
            createMiniTrees(*hypersuccinctTree, tree, fmMiniTrees, sizeMicro, bpsAndOccurrences);

            if(hypersuccinctTree->huffmanFlag) {
                std::map<std::vector<bool>,std::vector<bool>> huffmanTable = Huffman::generateTable<std::vector<bool>>(bpsAndOccurrences);
                convertToHuffman(*hypersuccinctTree, huffmanTable);
            }

            PHT_LOGGER_INFO("Factory Create", string("Finished Creating Hypersuccinct Tree"));

            //TODO: convert std::bool to BitVector
            convertToBitVector(hypersuccinctTree);

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
            createBitvectorFromFile(iter, end, hst.miniTypeVectors);
            createBitvectorFromFile(iter, end, hst.miniDummys);

            for(uint32_t j=0; j<miniTreesSize; j++) {
                MiniTree mini;
                createBitvectorFromFile(iter, end, mini.FIDs);
                createBitvectorFromFile(iter, end, mini.typeVectors);
                createBitvectorFromFile(iter, end, mini.dummys);
                createBitvectorFromFile(iter, end, mini.microTrees);
                createBitvectorFromFile(iter, end, mini.rootAncestors);
                createBitvectorFromFile(iter, end, mini.dummyAncestors);
                createBitvectorFromFile(iter, end, mini.miniDummyTree);
                createBitvectorFromFile(iter, end, mini.miniDummyIndex);
                createBitvectorFromFile(iter, end, mini.miniDummyPointer);
                createBitvectorFromFile(iter, end, mini.microDummyPointers);
                createBitvectorFromFile(iter, end, mini.miniAnc);
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
                Bitvector degree;
                createBitvectorFromFile(iter, end, degree);
                Bitvector subTrees;
                createBitvectorFromFile(iter, end, subTrees);
                Bitvector nodeDepths;
                createBitvectorFromFile(iter, end, nodeDepths);
                Bitvector nodeHeights;
                createBitvectorFromFile(iter, end, nodeHeights);
                Bitvector leaves;
                createBitvectorFromFile(iter, end, leaves);
                Bitvector leftmost_leaf;
                createBitvectorFromFile(iter, end, leftmost_leaf);
                Bitvector rightmost_leaf;
                createBitvectorFromFile(iter, end, rightmost_leaf);
                Bitvector leaf_rank;
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
            for(uint32_t i=0; i<tempSize; i++) {
                target.push_back(*iter);
                iter++;
            }
        }

        /**
         * This Class creates a Bitvector from a List of given UnorderedTrees
         * It is supposed to use a List of MicroTrees from the Farzan Munro Algorithm
         * Encoding consists of Elias Gamma Code of the size of the MicroTrees and their structure in Balanced Parenthesis form.
         *
         * @tparam T Class implemented in UnorderedTree
         * @param fmMicroTrees List of UnorderedTrees (MicroTrees)
         */
        template<class T> static Bitvector createBitVectorforMicroTrees(std::vector<std::shared_ptr<UnorderedTree<T>>> fmMicroTrees) {
            //determine size of Bitvector
            Bitvector res;
            for(std::shared_ptr<UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                int32_t size = fmMicroTree->getSize();
                //elias gamma code SIZE
                Bitvector_Utils::encodeNumber(std::inserter(res, res.end()), size,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                //BP FORM in bitform
                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                ListUtils::combine(res,bp);
            }
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
                std::vector<bool> oldEncodedMicros = x.microTrees;
                x.microTrees.clear();
                uint32_t entryCount = Bitvector_Utils::getEntryCount(oldEncodedMicros.cbegin(), oldEncodedMicros.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 2, 0});
                for(uint32_t i = 0; i < entryCount; i++) {
                    auto iter = oldEncodedMicros.cbegin();
                    std::vector<bool> bp = Bitvector_Utils::getEntry(iter, i, oldEncodedMicros.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 2, 0});
                    ListUtils::combine(x.microTrees, huffmanTable.at(bp));
                }
                HypersuccinctTreeOutput::printBitvector(x.microTrees);
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
        template<class T> static std::tuple<Bitvector,Bitvector> create1_2_Interconnections(std::shared_ptr<UnorderedTree<T>> baseTree, std::vector<std::shared_ptr<UnorderedTree<T>>> subtrees, uint32_t size) {
            Bitvector FIDs;
            Bitvector typeVectors;
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
                /*std::vector<std::pair<uint32_t,std::shared_ptr<Node<T>>>> enumResult;
                enumResult = ListUtils::mapped<std::shared_ptr<Node<T>>, std::pair<uint32_t,std::shared_ptr<Node<T>>>>(children, [&baseTree](std::shared_ptr<Node<T>> node){return std::pair<uint32_t,std::shared_ptr<Node<T>>>(baseTree->enumerate(node),node);});
                ListUtils::sort<std::pair<uint32_t,std::shared_ptr<Node<T>>>>(enumResult, [](std::pair<uint32_t,std::shared_ptr<Node<T>>> nodeA, std::pair<uint32_t,std::shared_ptr<Node<T>>> nodeB){return nodeA.first<nodeB.first;});
                children.clear();
                children = ListUtils::mapped<std::pair<uint32_t,std::shared_ptr<Node<T>>>, std::shared_ptr<Node<T>>>(enumResult, [](std::pair<uint32_t,std::shared_ptr<Node<T>>> nodeB){return nodeB.second;});*/
                Bitvector_Utils::encodeNumber(std::inserter(FIDs, FIDs.end()), children.size(),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                for(std::shared_ptr<Node<T>> node : children) {
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
        template<class T> static Bitvector createDummyInterconnections(std::shared_ptr<UnorderedTree<T>> baseTree, std::vector<std::shared_ptr<UnorderedTree<T>>> subtrees, uint32_t size) {
            Bitvector dummys;
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
                                Bitvector_Utils::encodeNumber(std::inserter(num, num.end()), fmMicroTree->enumerate(dummyNode),Bitvector_Utils::NumberEncoding::BINARY);
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

        /**
         * Fills the given Lookup Table entry with data from the given MicroTree
         * @tparam T Class implemented in UnorderedTree
         * @param lookupTableEntry The lookupTable Entry to fill
         * @param fmMicroTree The Micro Tree to get data from
         */
        template<class T> static void fillLookupTableEntry(LookupTableEntry& lookupTableEntry, const std::shared_ptr<UnorderedTree<T>>& fmMicroTree){
            PHT_LOGGER_INFO("Factory Create", string("Creating LookupTableEntries..."));
            std::vector<std::shared_ptr<Node<T>>> nodes = fmMicroTree->getNodes();
            //Generates LookupTable Entries
            for(std::shared_ptr<Node<T>> node1 : fmMicroTree->getNodes()) {

                std::vector<std::shared_ptr<pht::Node<T>>> directDesc = fmMicroTree->getDirectDescendants(node1);
                for(std::shared_ptr<Node<T>> node2 : fmMicroTree->getNodes()) {
                    lookupTableEntry.ancestorMatrix.push_back(fmMicroTree->isAncestor(node2, node1));
                    lookupTableEntry.childMatrix.push_back(ListUtils::containsAny(directDesc, {node2}));
                }

                uint32_t degreeNum = fmMicroTree->getDegree(node1) + 1;
                Bitvector_Utils::encodeNumber(lookupTableEntry.degree, degreeNum,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

                uint32_t subTreeNum = fmMicroTree->getSize(node1,true) + 1;
                Bitvector_Utils::encodeNumber(lookupTableEntry.subTrees,subTreeNum,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

                uint32_t nodeDepthNum = fmMicroTree->getDepth(node1, true) + 1;
                Bitvector_Utils::encodeNumber(lookupTableEntry.nodeDepths, nodeDepthNum,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

                uint32_t nodeHeightNum = fmMicroTree->getHeight(node1, true) + 1;
                Bitvector_Utils::encodeNumber(lookupTableEntry.nodeHeights, nodeHeightNum,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

                uint32_t leaveNum = fmMicroTree->getLeafSize(node1);
                Bitvector_Utils::encodeNumber(lookupTableEntry.leaves, leaveNum,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

                uint32_t leftmost_leafNum =  std::distance(nodes.begin(), std::find(nodes.begin(),nodes.end(),fmMicroTree->getLeftmostLeaf(node1)));
                Bitvector_Utils::encodeNumber(lookupTableEntry.leftmost_leaf, leftmost_leafNum,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

                uint32_t rightmost_leafNum = std::distance(nodes.begin(), std::find(nodes.begin(),nodes.end(),fmMicroTree->getRightmostLeaf(node1)));
                Bitvector_Utils::encodeNumber(lookupTableEntry.rightmost_leaf, rightmost_leafNum,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

                uint32_t leafRankNum = fmMicroTree->getLeafRank(node1) + 1;
                Bitvector_Utils::encodeNumber(lookupTableEntry.leafRank, leafRankNum,Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
            }
            PHT_LOGGER_INFO("Factory Create", string("Finished creating LookupTableEntries."));
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
        template<class T> static void createMicroTrees(HypersuccinctTree& hypersuccinctTree, MiniTree& miniTree, std::shared_ptr<UnorderedTree<T>>& fmMiniTree, std::vector<std::shared_ptr<UnorderedTree<T>>>& fmMicroTrees, std::map<std::vector<bool>,uint32_t>& bpsAndOccurrences,uint32_t sizeMicro){
            PHT_LOGGER_INFO("Factory Create", string("Creating MicroTrees for a MiniTree..."));
            uint32_t microCount = 0;
            //The actual MicroTree Loop
            //Put everything that needs MicroTree Iteration in this loop
            for(const std::shared_ptr<UnorderedTree<T>>& fmMicroTree : fmMicroTrees) {

                handleMiniDummyInMicroTree(miniTree, fmMiniTree, fmMicroTree, fmMicroTrees);

                //Dummy Ancestory
                if(fmMiniTree->hasDummy()) {
                    if(fmMicroTree->hasDummy()) {
                        miniTree.dummyAncestors.push_back(fmMiniTree->isAncestor(fmMiniTree->getDummy(), fmMicroTree->getDummy()));
                    } else {
                        miniTree.dummyAncestors.push_back(false);
                    }
                    miniTree.rootAncestors.push_back(fmMiniTree->isAncestor(fmMiniTree->getDummy(), fmMicroTree->getRoot()));
                }

                //MicroDummyPointers
                uint32_t dummySize = floor(log2(2*sizeMicro+1))+1;
                if(fmMicroTree->hasDummy()) {
                    std::shared_ptr<Node<T>> dummyPoint = fmMiniTree->getDirectDescendants(fmMicroTree->getDummy()).at(0);
                    uint32_t microTreePointer = dummyPoint->getMicroTree();
                    Bitvector microDummyP;
                    Bitvector_Utils::encodeNumber(microDummyP, microTreePointer, Bitvector_Utils::NumberEncoding::BINARY);
                    for(uint32_t i=0; i<dummySize-microDummyP.size(); i++) {
                        miniTree.microDummyPointers.push_back(false);
                    }
                    ListUtils::combine(miniTree.microDummyPointers,microDummyP);
                } else {
                    for(uint32_t i=0; i<dummySize;i++) {
                        miniTree.microDummyPointers.push_back(false);
                    }
                }

                //Simple Additions for Queries - MicroTrees
                /*Bitvector microSubTree; TODO: Check if Full Elias Gamma is more efficient
                Bitvector_Utils::encodeNumber(microSubTree, fmMiniTree->getSize(fmMicroTree->getRoot(),false),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                Bitvector_Utils::encodeNumber(miniTree.microSubTrees, microSubTree.size(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                ListUtils::combine(miniTree.microSubTrees,microSubTree);*/



                Bitvector_Utils::encodeNumber(miniTree.microSubTrees, fmMiniTree->getSize(fmMicroTree->getRoot(),false),Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                Bitvector_Utils::encodeNumber(miniTree.rootDepths, fmMiniTree->getDepth(fmMicroTree->getRoot())+1, Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                Bitvector_Utils::encodeNumber(miniTree.rootHeights, fmMiniTree->getHeight(fmMicroTree->getRoot())+1, Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                Bitvector_Utils::encodeNumber(miniTree.microLeaves, fmMiniTree->getLeafSize(fmMicroTree->getRoot()), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                Bitvector_Utils::encodeNumber(miniTree.microTreeLeftmostLeafPointers, fmMiniTree->getLeftmostLeaf(fmMicroTree->getRoot())->getMicroTree(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                Bitvector_Utils::encodeNumber(miniTree.microTreeRightmostLeafPointers, fmMiniTree->getRightmostLeaf(fmMicroTree->getRoot())->getMicroTree(), Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                Bitvector_Utils::encodeNumber(miniTree.microRootLeafRanks, fmMiniTree->getLeafRank(fmMicroTree->getRoot())+1, Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);

                Bitvector bp = fmMicroTree->toBalancedParenthesis();
                if(hypersuccinctTree.huffmanFlag) {
                    if(bpsAndOccurrences.find(bp) == bpsAndOccurrences.end()) {
                        bpsAndOccurrences.insert({bp, 0});
                    }
                    bpsAndOccurrences.at(bp)++;
                }

                LookupTableEntry microTreeData(bp);
                //TODO: We can only accept MicroTrees WITHOUT Dummys (due to consistency)
                //TODO: We can only accept MicroTree with Dummys if it is the only MicroTree with that shape
                if(!ListUtils::containsAny(hypersuccinctTree.lookupTable, {microTreeData})) {
                    fillLookupTableEntry(microTreeData, fmMicroTree);
                    hypersuccinctTree.lookupTable.push_back(microTreeData);
                }


                std::pair<uint32_t ,uint32_t > microFIDIndices = convertMicroTreeToFIDIndex(hypersuccinctTree,miniTree,microCount);
                Bitvector_Utils::encodeNumber(miniTree.microTopFIDIndices, microFIDIndices.first+2, Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                Bitvector_Utils::encodeNumber(miniTree.microLowFIDIndices, microFIDIndices.second+2, Bitvector_Utils::NumberEncoding::ELIAS_GAMMA);
                microCount++;
            }
            PHT_LOGGER_INFO("Factory Create", string("Finished Creating MicroTrees for this MiniTree."));
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
        template<class T> static void createMiniTrees(HypersuccinctTree& hypersuccinctTree, const std::shared_ptr<UnorderedTree<T>>& tree, std::vector<std::shared_ptr<UnorderedTree<T>>>& fmMiniTrees, uint32_t sizeMicro, std::map<std::vector<bool>,uint32_t>& bpsAndOccurrences){

            PHT_LOGGER_INFO("Factory Create", string("Creating MiniTrees..."));
            for(std::shared_ptr<UnorderedTree<T>> fmMiniTree : fmMiniTrees) {
                std::vector<std::shared_ptr<UnorderedTree<T>>> fmMicroTrees = FarzanMunro<T>::decompose(fmMiniTree, sizeMicro);
                std::cout << "Test" << std::endl;
                MiniTree miniTree = MiniTree();
                std::cout << "TestEnd" << std::endl;

                //Creating Micro Interconnections and Dummys
                std::tie(miniTree.FIDs, miniTree.typeVectors) = create1_2_Interconnections(fmMiniTree,fmMicroTrees,sizeMicro);
                Bitvector dummys = createDummyInterconnections(fmMiniTree, fmMicroTrees, sizeMicro);
                miniTree.dummys = dummys;

                enumerateMicroTrees(fmMicroTrees);

                //Simple Additions for Queries - MiniTree
                Bitvector_Utils::encodeNumber(miniTree.subTree, tree->getSize(fmMiniTree->getRoot(),false),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniDepth, tree->getDepth(fmMiniTree->getRoot()), Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniHeight, tree->getHeight(fmMiniTree->getRoot()), Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniLeaves, tree->getLeafSize(fmMiniTree->getRoot()),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniTreeLeftmostLeafPointer, tree->getLeftmostLeaf(fmMiniTree->getRoot())->getMiniTree(),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniTreeRightmostLeafPointer, tree->getRightmostLeaf(fmMiniTree->getRoot())->getMiniTree(),Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniRootLeafRank, tree->getLeafRank(fmMiniTree->getRoot()),Bitvector_Utils::NumberEncoding::BINARY);


                //MiniDummy Bitvectors
                if(fmMiniTree->hasDummy()) {
                    std::shared_ptr<Node<T>> dummyPoint = tree->getDirectDescendants(fmMiniTree->getDummy()).at(0);
                    uint32_t miniTreePointer = dummyPoint->getMiniTree();
                    Bitvector_Utils::encodeNumber(miniTree.miniDummyPointer, miniTreePointer, Bitvector_Utils::NumberEncoding::BINARY);
                    Bitvector_Utils::encodeNumber(miniTree.miniDummyDepth, tree->getDepth(dummyPoint), Bitvector_Utils::NumberEncoding::BINARY);
                    Bitvector_Utils::encodeNumber(miniTree.miniDummyHeight, tree->getHeight(dummyPoint), Bitvector_Utils::NumberEncoding::BINARY);
                    Bitvector_Utils::encodeNumber(miniTree.miniDummyLeafRank, tree->getLeafRank(dummyPoint),Bitvector_Utils::NumberEncoding::BINARY);
                }

                createMicroTrees(hypersuccinctTree, miniTree, fmMiniTree, fmMicroTrees, bpsAndOccurrences,sizeMicro);

                //This is done so late because of Huffman checks
                miniTree.microTrees = createBitVectorforMicroTrees(fmMicroTrees);
                hypersuccinctTree.miniTrees.push_back(miniTree);

                //Output
                //TODO: Remove or put in Logger
                std::cout << "Size of MiniTree: " << fmMiniTree->getSize() << "\n";
                std::cout << "Root of MiniTree: " << fmMiniTree->getRoot()->getValue() << "\n";
                std::cout << "Nodes of MiniTree: " << fmMiniTree->toNewickString() << "\n";
                std::cout << "Amount of MicroTrees: " << fmMicroTrees.size() << "\n";
                for(std::shared_ptr<UnorderedTree<std::string>>& fmMicroTree : fmMicroTrees) {
                    std::cout << "Size of MicroTree: " << fmMicroTree->getSize() << "\n";
                    std::cout << "Root of MicroTree: " << fmMicroTree->getRoot()->getValue() << "\n";
                    std::cout << "Nodes of MicroTree: " << fmMicroTree->toNewickString() << "\n";
                }
                std::cout << std::endl;
            }

            uint32_t miniSize = hypersuccinctTree.miniTrees.size();
            for(uint32_t i=0; i<miniSize; i++) {
                std::pair<uint32_t ,uint32_t > miniFIDIndices = convertTreeToFIDIndex(hypersuccinctTree,i);
                MiniTree& miniTree = hypersuccinctTree.getMiniTree(i);
                Bitvector_Utils::encodeNumber(miniTree.miniTopFIDIndex, miniFIDIndices.first+1,Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector_Utils::encodeNumber(miniTree.miniLowFIDIndex, miniFIDIndices.second+1,Bitvector_Utils::NumberEncoding::BINARY);
            }
            PHT_LOGGER_INFO("Factory Create", string("Finished Creating MiniTrees."));
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
                std::vector<std::shared_ptr<Node<T>>> nodes = fmMicroTree->getNodes();
                auto iter1 = std::find(nodes.begin(),nodes.end(), fmMiniTree->getDummy());
                dist = std::distance(nodes.begin(), iter1);
                Bitvector_Utils::encodeNumber(miniTree.miniDummyIndex,dist,Bitvector_Utils::NumberEncoding::BINARY);
            }
        }




        static std::pair<uint32_t ,uint32_t > convertTreeToFIDIndex(const HypersuccinctTree& hypersuccinctTree, uint32_t miniTree) {
            auto iterD = hypersuccinctTree.miniFIDs.cbegin();
            std::vector<Bitvector> fids;
            fids.push_back(Bitvector_Utils::getEntry(iterD, 0, hypersuccinctTree.miniFIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 1, 0}));

            auto iterD2 = hypersuccinctTree.miniTypeVectors.cbegin();
            auto iterF = hypersuccinctTree.miniFIDs.cbegin();
            std::vector<Bitvector> tvs;
            tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, hypersuccinctTree.miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, hypersuccinctTree.miniFIDs.cend(), 2, 0}));

            std::vector<Bitvector>dummys;
            auto iter = hypersuccinctTree.miniSize.cbegin();
            uint32_t miniSizeNum = pht::Bitvector_Utils::decodeNumber(iter, hypersuccinctTree.miniSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t dummySize = floor(log2(2*miniSizeNum+1))+1;
            auto iterD3 = hypersuccinctTree.miniDummys.cbegin();
            dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, hypersuccinctTree.miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),0, dummySize}));

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
                bool found = false;
                for(int i=0; i<childFIDs.size(); i++) {
                    childIndex += childFIDs.at(i);
                    if(currentIndex <= childIndex) {
                        found = true;
                        if(miniTree < topOffset + topTrees) {
                            lowIndex = i;
                        }
                        break;
                    }
                }
                if(currentIndex > 0 && !found) {
                    childFIDs.push_back(topTrees + lowTrees);
                }
                else {
                    childFIDs.push_back(lowTrees);
                }

                //TopTree Index Conversion
                if(miniTree < topOffset + topTrees) {
                    topIndex = currentIndex;
                    return {topIndex,lowIndex};
                }
                topOffset += topTrees;

                //Getting new FIDs
                if(iterD != hypersuccinctTree.miniFIDs.cend()) {
                    for (uint32_t i = 0; i < lowTrees; i++) {
                        fids.push_back(Bitvector_Utils::getEntry(iterD, 0, hypersuccinctTree.miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),1, 0}));
                        tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, hypersuccinctTree.miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, hypersuccinctTree.miniFIDs.cend(), 2, 0}));
                        dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, hypersuccinctTree.miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),0, dummySize}));
                    }
                }
                auto iterDummy = dummys.at(currentIndex).cbegin();
                if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    fids.push_back(Bitvector_Utils::getEntry(iterD, 0, hypersuccinctTree.miniFIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),1, 0}));
                    tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, hypersuccinctTree.miniTypeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, hypersuccinctTree.miniFIDs.cend(), 2, 0}));
                    dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, hypersuccinctTree.miniDummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),0, dummySize}));
                }
                currentIndex++;
            }
            return {-1,-1};
        }

        static std::pair<uint32_t ,uint32_t > convertMicroTreeToFIDIndex(const HypersuccinctTree& hypersuccinctTree, MiniTree &miniTree, uint32_t microTree) {
            auto iterD = miniTree.FIDs.cbegin();
            std::vector<Bitvector> fids;
            fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniTree.FIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 1, 0}));

            auto iterD2 = miniTree.typeVectors.cbegin();
            auto iterF = miniTree.FIDs.cbegin();
            std::vector<Bitvector> tvs;
            tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniTree.FIDs.cend(), 2, 0}));

            std::vector<Bitvector>dummys;
            auto iter = hypersuccinctTree.microSize.cbegin();
            uint32_t miniSizeNum = pht::Bitvector_Utils::decodeNumber(iter, hypersuccinctTree.microSize.cend(),Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t dummySize = floor(log2(2*miniSizeNum+1))+1;
            auto iterD3 = miniTree.dummys.cbegin();
            dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniTree.dummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),0, dummySize}));

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
                bool found = false;
                for(int i=0; i<childFIDs.size(); i++) {
                    childIndex += childFIDs.at(i);
                    if(currentIndex <= childIndex) {
                        found = true;
                        if(microTree < topOffset + topTrees) {
                            lowIndex = i;
                        }
                        break;
                    }
                }
                if(currentIndex > 0 && !found) {
                    childFIDs.push_back(topTrees + lowTrees);
                }
                else {
                    childFIDs.push_back(lowTrees);
                }

                //TopTree Index Conversion
                if(microTree < topOffset + topTrees) {
                    topIndex = currentIndex;
                    return {topIndex,lowIndex};
                }
                topOffset += topTrees;

                //Getting new FIDs
                if(iterD != miniTree.FIDs.cend()) {
                    for (uint32_t i = 0; i < lowTrees; i++) {
                        fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniTree.FIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),1, 0}));
                        tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniTree.FIDs.cend(), 2, 0}));
                        dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniTree.dummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),0, dummySize}));
                    }
                }
                auto iterDummy = dummys.at(currentIndex).cbegin();
                if(pht::Bitvector_Utils::decodeNumber(iterDummy, dummys.at(currentIndex).cend(),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    fids.push_back(Bitvector_Utils::getEntry(iterD, 0, miniTree.FIDs.cend(),Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,{Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),1, 0}));
                    tvs.push_back(Bitvector_Utils::getEntry(iterD2, 0, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniTree.FIDs.cend(), 2, 0}));
                    dummys.push_back(Bitvector_Utils::getEntry(iterD3, 0, miniTree.dummys.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(),0, dummySize}));
                }
                currentIndex++;
            }
            return {-1,-1};
        }

        static void convertToBitVector(HypersuccinctTree& hst) {
            assignBitVector(hst.miniFIDsSupport , hst.miniFIDs);
            assignBitVector(hst.miniTypeVectorsSupport , hst.miniTypeVectors);
            assignBitVector(hst.miniDummysSupport , hst.miniDummys);
            for(MiniTree &miniTree: hst.getMiniTrees()) {
                assignBitVector(miniTree.FIDsSupport , miniTree.FIDs);
                assignBitVector(miniTree.typeVectorsSupport , miniTree.typeVectors);
                assignBitVector(miniTree.dummysSupport , miniTree.dummys);
                assignBitVector(miniTree.microTreesSupport , miniTree.microTrees);
                assignBitVector(miniTree.miniTopFIDIndexSupport , miniTree.miniTopFIDIndex);
                assignBitVector(miniTree.miniLowFIDIndexSupport , miniTree.miniLowFIDIndex);
                assignBitVector(miniTree.microTopFIDIndicesSupport , miniTree.microTopFIDIndices);
                assignBitVector(miniTree.microLowFIDIndicesSupport , miniTree.microLowFIDIndices);
                assignBitVector(miniTree.rootAncestorsSupport , miniTree.rootAncestors);
                assignBitVector(miniTree.dummyAncestorsSupport , miniTree.dummyAncestors);
                assignBitVector(miniTree.miniDummyTreeSupport , miniTree.miniDummyTree);
                assignBitVector(miniTree.miniDummyIndexSupport , miniTree.miniDummyIndex);
                assignBitVector(miniTree.miniDummyPointerSupport , miniTree.miniDummyPointer);
                assignBitVector(miniTree.microDummyPointersSupport , miniTree.microDummyPointers);
                assignBitVector(miniTree.miniAncSupport , miniTree.miniAnc);
                assignBitVector(miniTree.subTreeSupport , miniTree.subTree);
                assignBitVector(miniTree.microSubTreesSupport , miniTree.microSubTrees);
                assignBitVector(miniTree.miniDepthSupport , miniTree.miniDepth);
                assignBitVector(miniTree.miniHeightSupport , miniTree.miniHeight);
                assignBitVector(miniTree.miniDummyDepthSupport , miniTree.miniDummyDepth);
                assignBitVector(miniTree.miniDummyHeightSupport , miniTree.miniDummyHeight);
                assignBitVector(miniTree.rootDepthsSupport , miniTree.rootDepths);
                assignBitVector(miniTree.rootHeightsSupport , miniTree.rootHeights);
                assignBitVector(miniTree.miniLeavesSupport , miniTree.miniLeaves);
                assignBitVector(miniTree.microLeavesSupport , miniTree.microLeaves);
                assignBitVector(miniTree.miniTreeLeftmostLeafPointerSupport , miniTree.miniTreeLeftmostLeafPointer);
                assignBitVector(miniTree.miniTreeRightmostLeafPointerSupport , miniTree.miniTreeRightmostLeafPointer);
                assignBitVector(miniTree.microTreeLeftmostLeafPointersSupport , miniTree.microTreeLeftmostLeafPointers);
                assignBitVector(miniTree.microTreeRightmostLeafPointersSupport , miniTree.microTreeRightmostLeafPointers);
                assignBitVector(miniTree.miniRootLeafRankSupport , miniTree.miniRootLeafRank);
                assignBitVector(miniTree.miniDummyLeafRankSupport , miniTree.miniDummyLeafRank);
                assignBitVector(miniTree.microRootLeafRanksSupport , miniTree.microRootLeafRanks);
            }
            for(LookupTableEntry &entry : hst.lookupTable) {
                assignBitVector(entry.indexSupport , entry.index);
                assignBitVector(entry.bpSupport , entry.bp);
                assignBitVector(entry.ancestorMatrixSupport , entry.ancestorMatrix);
                assignBitVector(entry.childMatrixSupport , entry.childMatrix);
                assignBitVector(entry.degreeSupport , entry.degree);
                assignBitVector(entry.subTreesSupport , entry.subTrees);
                assignBitVector(entry.nodeDepthsSupport , entry.nodeDepths);
                assignBitVector(entry.nodeHeightsSupport , entry.nodeHeights);
                assignBitVector(entry.leavesSupport , entry.leaves);
                assignBitVector(entry.leftmost_leafSupport , entry.leftmost_leaf);
                assignBitVector(entry.rightmost_leafSupport , entry.rightmost_leaf);
                assignBitVector(entry.leafRankSupport , entry.leafRank);
            }
        }

        static void assignBitVector(succinct_bv::BitVector& bitVector, const Bitvector& bitvector) {
            if(!bitvector.empty()) {
                bitVector = bitvector;
            }
        }
    };
}

#undef DLL_API
#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H