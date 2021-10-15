#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H

#include <iostream>
#include <functional>
#include <utility>

#include "unordered_tree.h"
#include "list_utils.h"
#include "farzan_munro.h"
#include "bitvector_utils.h"
#include "bit_vector.h"
#include "hst_node.h"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

namespace pht {
    typedef std::vector<bool> Bitvector;
    //node identification by: Minitree, MicroTree, NodeInMicroTree

    /**
     * MiniTree represents MiniTree of the HypersuccinctTree
     * It contains all information needed to query a single MiniTree.
     * TODO: ALL vectors get +1 to their numbers - so 0 is clear illegal value!
     */
    struct __declspec(dllexport) MiniTree {
        //MicroFIDs
        std::vector<Bitvector> FIDs;
        std::vector<succinct_bv::BitVector> FIDsSupport;
        //MicroTypeVectors
        std::vector<Bitvector> typeVectors;
        std::vector<succinct_bv::BitVector> typeVectorsSupport;
        //MicroDummys: Static Size Encoding
        std::vector<Bitvector> dummys;
        std::vector<succinct_bv::BitVector> dummysSupport;
        //MircoTrees as encoded (BP if no encoding, huffman code if huffman encoding)
        std::vector<Bitvector> microTrees;
        std::vector<succinct_bv::BitVector> microTreesSupport;

        //MiniTree Top FID Index + 1
        Bitvector miniTopFIDIndex;
        succinct_bv::BitVector miniTopFIDIndexSupport;
        //MiniTree Low FID Index + 1
        Bitvector miniLowFIDIndex;
        succinct_bv::BitVector miniLowFIDIndexSupport;
        //MicroTree Top FID Indices + 1
        std::vector<Bitvector> microTopFIDIndices;
        std::vector<succinct_bv::BitVector> microTopFIDIndicesSupport;
        //MicroTree Low FID Indices + 1
        std::vector<Bitvector> microLowFIDIndices;
        std::vector<succinct_bv::BitVector> microLowFIDIndicesSupport;
        //First Top Tree of MicroFIDs
        std::vector<Bitvector> microFIDTopTrees;
        std::vector<succinct_bv::BitVector> microFIDTopTreesSupport;
        //First Low Tree of MicroFIDs
        std::vector<Bitvector> microFIDLowTrees;
        std::vector<succinct_bv::BitVector> microFIDLowTreesSupport;
        std::vector<Bitvector> microFIDLowTreeAmount;


        //Is MicroTree root ancestor of MiniTreeDummy? empty/0 if no MiniDummy exists
        Bitvector rootAncestors;
        succinct_bv::BitVector rootAncestorsSupport;
        //Is MicroTreeDummy ancestor of MiniTreeDummy? 0 per entry if no MicroDummy exists, empty/0 if no MiniDummy exists
        Bitvector dummyAncestors;
        succinct_bv::BitVector dummyAncestorsSupport;
        //If MiniTree has Dummy: Which MicroTree contains this Dummy?
        Bitvector miniDummyTree;
        succinct_bv::BitVector miniDummyTreeSupport;
        //If MiniTree has Dummy: Which Index within the MicroTree is this Dummy?
        Bitvector miniDummyIndex;
        succinct_bv::BitVector miniDummyIndexSupport;
        //If MiniTree has Dummy: To which Tree does the pointer lead?
        Bitvector miniDummyPointer;
        succinct_bv::BitVector miniDummyPointerSupport;
        //MicroTree Dummy Pointer: Static Size Encoding
        std::vector<Bitvector> microDummyPointers;
        std::vector<succinct_bv::BitVector> microDummyPointersSupport;

        //Child Rankd of MiniTrees
        Bitvector miniChildRank;
        succinct_bv::BitVector miniChildRankSupport;
        //Child Ranks of MicroTrees EXCEPT AT INDEX 0: ChildRank of displaced MiniTree
        std::vector<Bitvector> microChildRanks;
        std::vector<succinct_bv::BitVector> microChildRanksSupport;
        //Child Ranks of displaced MicroTrees
        std::vector<Bitvector> microExtendedChildRanks;
        std::vector<succinct_bv::BitVector> microExtendedChildRanksSupport;

        //Pointer to direct parent MiniTree
        Bitvector miniParent;
        std::vector<succinct_bv::BitVector> miniParentSupport;
        //Pointer to direct parent MicroTree for all MicroTrees
        std::vector<Bitvector> microParents;
        std::vector<succinct_bv::BitVector> microParentsSupport;

        //SubTree Size MiniTree
        Bitvector subTree;
        succinct_bv::BitVector subTreeSupport;
        //SubTree Size for MicroTree roots
        std::vector<Bitvector> microSubTrees;
        std::vector<succinct_bv::BitVector> microSubTreesSupport;
        //Depth of the MiniTree root
        Bitvector miniDepth;
        succinct_bv::BitVector miniDepthSupport;
        //Height of the MiniTree root
        Bitvector miniHeight;
        succinct_bv::BitVector miniHeightSupport;
        //Depth of the MiniTree Dummy
        Bitvector miniDummyDepth;
        succinct_bv::BitVector miniDummyDepthSupport;
        //Height of the MiniTree Dummy
        Bitvector miniDummyHeight;
        succinct_bv::BitVector miniDummyHeightSupport;
        //Depths for MicroTree roots + 1
        std::vector<Bitvector> rootDepths;
        std::vector<succinct_bv::BitVector> rootDepthsSupport;
        //Heights for MicroTree roots + 1
        std::vector<Bitvector> rootHeights;
        std::vector<succinct_bv::BitVector> rootHeightsSupport;
        //Amount of Leaves in MiniTree
        Bitvector miniLeaves;
        succinct_bv::BitVector miniLeavesSupport;
        //Amount of Leaves within MicroTrees
        std::vector<Bitvector> microLeaves;
        std::vector<succinct_bv::BitVector> microLeavesSupport;
        //Leftmost Leaf Pointer for MiniTree
        Bitvector miniTreeLeftmostLeafPointer;
        succinct_bv::BitVector miniTreeLeftmostLeafPointerSupport;
        //Rightmost Leaf Pointer for MiniTree
        Bitvector miniTreeRightmostLeafPointer;
        succinct_bv::BitVector miniTreeRightmostLeafPointerSupport;
        //Leftmost Leaf Pointers for MicroTrees
        std::vector<Bitvector> microTreeLeftmostLeafPointers;
        std::vector<succinct_bv::BitVector> microTreeLeftmostLeafPointersSupport;
        //Rightmost Leaf Pointers for MicroTrees
        std::vector<Bitvector> microTreeRightmostLeafPointers;
        std::vector<succinct_bv::BitVector> microTreeRightmostLeafPointersSupport;
        //Leaf Rank of MiniTree Root
        Bitvector miniRootLeafRank;
        succinct_bv::BitVector miniRootLeafRankSupport;
        //Leaf Rank of MiniTree Dummy
        Bitvector miniDummyLeafRank;
        succinct_bv::BitVector miniDummyLeafRankSupport;
        //Leaf Ranks of MicroTree Roots + 1 EXCEPT AT INDEX 0: There it is the displaced Leaf Rank of the MiniTree
        std::vector<Bitvector> microRootLeafRanks;
        std::vector<succinct_bv::BitVector> microRootLeafRanksSupport;
        //Special Leaf Ranks for displaced microTrees + 1
        std::vector<Bitvector> microExtendedLeafRanks;
        std::vector<succinct_bv::BitVector> microExtendedLeafRanksSupport;
    };

    /**
     * LookupTableEntry represents a single Entry of the Hypersuccinct Tree's Lookp Table.
     * It is indexed by the MicroTrees Balanced Parenthesis form (if no encoding is chosen) or by their Huffman code (if Huffman encoding is chosen)
     * It contains all fields necessary to satisfy the query's need for structural information
     */
    struct __declspec(dllexport) LookupTableEntry {
        //Index of the LookupTableEntry
        Bitvector index;
        succinct_bv::BitVector indexSupport;
        //BP of the Entry. Empty if index is BP
        Bitvector bp;
        succinct_bv::BitVector bpSupport;
        //Ancestor Matrix
        Bitvector ancestorMatrix;
        succinct_bv::BitVector ancestorMatrixSupport;
        //Child Matrix
        Bitvector childMatrix;
        succinct_bv::BitVector childMatrixSupport;
        //ParentPointers + 1
        std::vector<Bitvector> parentPointers;
        std::vector<succinct_bv::BitVector> parentPointersSupport;
        //degree for every node + 1
        std::vector<Bitvector> degree;
        std::vector<succinct_bv::BitVector> degreeSupport;
        //subTree for every node within MicroTree (at least 1)
        std::vector<Bitvector> subTrees;
        std::vector<succinct_bv::BitVector> subTreesSupport;
        //Depths of nodes + 1
        std::vector<Bitvector> nodeDepths;
        std::vector<succinct_bv::BitVector> nodeDepthsSupport;
        //Heights of nodes + 1
        std::vector<Bitvector> nodeHeights;
        std::vector<succinct_bv::BitVector> nodeHeightsSupport;
        //Amount of Leaves for every node within MicroTree (at least 1)
        std::vector<Bitvector> leaves;
        std::vector<succinct_bv::BitVector> leavesSupport;
        //Rightmost leaves for every node within MicroTree
        std::vector<Bitvector> leftmost_leaf;
        std::vector<succinct_bv::BitVector> leftmost_leafSupport;
        //Leftmose leaves for every node within MicroTree
        std::vector<Bitvector> rightmost_leaf;
        std::vector<succinct_bv::BitVector> rightmost_leafSupport;
        //Leaf Rank for every node within MicroTree + 1
        std::vector<Bitvector> leafRank;
        std::vector<succinct_bv::BitVector> leafRankSupport;


        //TODO: This constructor is specifically for HypersuccinctTreeFactory - could be removed
        LookupTableEntry(const Bitvector& index) : index(index) {}
        //TODO: Extend constructor as more fields are added!
        LookupTableEntry(const Bitvector& index, const Bitvector& bp) : index(index), bp(bp) {}
        bool operator==(const LookupTableEntry& mtd) const {
            return index == mtd.index;
        }
    };


    /**
     * This class represents the full hypersuccinct code for any given tree, created by HypersuccinctTreeFactory
     * as specified in 'A Uniform Paradigm to Succinctly Encode Various Families of Trees' by Arash Farzan; J. Ian Munro.
     * All code is represented as Bitvectors
     * It can be encoded with huffman encoding for MicroTrees
     * TODO: Need Complexity for all functions
     *
     * It contains:
     * The FarzanMunro Algorithm Sizes of MiniTrees and MicroTrees
     * The MiniTrees as a vector of MiniTree
     * The MiniTree FIDs, TypeVectors, and Dummys
     * The LookupTable as a vector of LookUpTableEntry
     *
     * This class implements:
     * Get functions for all Bitvectors
     * Queries as specified in 'A Uniform Paradigm to Succinctly Encode Various Families of Trees' by Arash Farzan; J. Ian Munro
     */
    class __declspec(dllexport) HypersuccinctTree {
        friend class HypersuccinctTreeFactory;
    public:

        /**
         * Returns the MiniTree at the given index
         * O(1)
         *
         * @param index The Index of the MiniTree as integer
         * @return MiniTree as MiniTree
         */
        MiniTree& getMiniTree(uint32_t index ) {
            return miniTrees.at(index);
        }

        /**
         * Checks if the Tree is encoded with Huffman encoding
         * O(1)
         *
         * @return value of huffmanFlag
         */
        [[nodiscard]] bool isHuffman() const {
            return huffmanFlag;
        }

        Bitvector getSize() {
            return size;
        }

        Bitvector getMicroSize() {
            return microSize;
        }

        Bitvector getMiniSize() {
            return miniSize;
        }

        std::vector<MiniTree>& getMiniTrees() {
            return miniTrees;
        }

        std::vector<Bitvector> getMiniFIDs() {
            return miniFIDs;
        }

        std::vector<Bitvector> getFIDTopTrees() {
            return miniFIDTopTree;
        }

        std::vector<Bitvector> getFIDLowTrees() {
            return miniFIDLowTree;
        }

        std::vector<Bitvector> getMiniTypeVectors() {
            return miniTypeVectors;
        }

        std::vector<Bitvector> getMiniDummys() {
            return miniDummys;
        }

        std::vector<LookupTableEntry> getLookupTable() {
            return lookupTable;
        }

        /**
         * Returns the MicroTree Entry of the given MiniTree at the given MicroTree Index
         * Always returns Balanced parenthesis form of the MicroTree, regardless of encoding (Huffman or not)
         *
         * @param miniTree The MiniTree of the MicroTree
         * @param index The index of the MicroTree as int
         * @return the MicroTree in Balanced Parenthesis form as bitvector
         */
        Bitvector getMicroTree(MiniTree& miniTree,uint32_t index);

        uint32_t getMicroTreeCount(MiniTree& miniTree);

        /**
         * Returns the MicroFID Entry of the given MiniTree at the given MicroTree Index
         *
         * @param miniTree The MiniTree of the MicroFID
         * @param index The index of the MicroTree as int
         * @return the FID Entry as bitvector
         */
        Bitvector getMicroFID(MiniTree& miniTree,uint32_t index);

        /**
         * Returns the TypeVector Entry of the given MiniTree at the given MicroTree Index
         *
         * @param miniTree The MiniTree of the TypeVector
         * @param index The index of the MicroTree es int
         * @return the Typevector Entry as bitvector
         */
        Bitvector getMicroTypeVector(MiniTree& miniTree , uint32_t index);

        /**
         * Returns the MicroDummy Entry from the perspective of the MicroTree
         *
         * @param miniTree The MiniTree of the Dummy
         * @param index The index of the MicroTree as int
         * @return the Dummy Entry as bitvector
         */
        Bitvector getMicroDummys(MiniTree& miniTree, uint32_t index);

        /**
         * Returns the MicroDummy Pointer Entry for a given MicroTree
         *
         * @param miniTree The MiniTree of the Dummy
         * @param index The index of the MicroTree
         * @return The Dummy Pointed MicroTree as int
         */
        Bitvector getMicroDummyPointers(MiniTree& miniTree, uint32_t index);

        /**
         * Returns the MiniDummy Entry from the perspective of the MiniTree
         *
         * @param index The index of the MiniTree as int
         * @return The Dummy Entry as bitvector
         */
        Bitvector getMiniDummy(uint32_t index);

        /**
         * Returns the LookupTable Entry at the given index
         *
         * @param index The index as int
         * @return LookupTable Entry as LookupTableEntry
         */
        LookupTableEntry& getLookupTableEntry(uint32_t index) {
            return lookupTable.at(index);
        }



        /**
         * Returns the LookupTable Entry of the given MicroTree
         *
         * @param indexV The MicroTree as bitvector from a miniTree
         * @return LookupTable Entry of the MicroTree as LookupTableEntry
         */
        LookupTableEntry getLookupTableEntry(Bitvector indexV);

        /**
         * Returns if node1 is ancestor of node2 with the given LookupTable Entry
         * Both nodes only need their index inside the MicroTree (HstNode index 2)
         *
         * @param entry The LookupTable Entry
         * @param anc Index of Node 1
         * @param node2Index Index of Node 2
         * @return if node1 is ancestor of node2 as bool
         */
        bool lookupTableAncestorMatrixComparison(const LookupTableEntry& entry, uint32_t anc, uint32_t node2Index);

        /**
         * Returns if node1 is child of node2 with the given LookupTable Entry
         * Both nodes only need their index inside the MicroTree (HstNode index 2)
         *
         * @param entry The LookupTable Entry
         * @param anc Index of Node 1
         * @param node2Index Index of Node 2
         * @return if node1 is ancestor of node2 as bool
         */
        bool lookupTableChildMatrixComparison(const LookupTableEntry& entry, uint32_t child, uint32_t node2Index);

        /**
         * Returns if given Node is ancestor of Dummy within the Node's MiniTree
         * TODO: Look at the Micro Version - This can clearly be optimized
         *
         * @param node The Node as HstNode
         * @return if Node is ancestor of MiniDummy as bool
         */
        bool isDummyAncestorWithinMiniTree(HstNode node);

        /**
         * Returns if  given Node is ancestor of Dummy within the Node's MicroTree
         *
         * @param node  The Node as HstNode
         * @return if Node is ancestor of MicroDummy as bool
         */
        bool isDummyAncestorWithinMicroTree(HstNode node);

        /**
         * Returns the ith child of a given Node, if it exists
         *
         * @param parent The parent Node as HStNode
         * @param index The index as unint32_t
         * @return The ith child as HstNode
         */
        HstNode child(HstNode parent, uint32_t index);

        /**
         * Returns the Child Rank of a given Node
         *
         * @param node The Node as HstNode
         * @return The Child Rank as int
         */
        uint32_t childRank(HstNode node);

        /**
         * Finds the direct Parent of the given Node, ignoring dummies
         * Important hepler function
         * TODO: Should be private
         *
         * @param node The node as HSTNode
         * @return The parent as HstNode
         */
        HstNode getParentForQuery(HstNode node);

        /**
         * Finds the direct Parent of the given Node
         *
         * @param node The node as HSTNode
         * @return The parent as HstNode
         */
        HstNode getParent(HstNode node);

        /**
         * Returns the degree of a given Node
         *
         * @param node The Node as HstNode
         * @return the degree as int
         */
        uint32_t degree(HstNode node);

        /**
         * Returns the Subtree size of a given Node
         *
         * @param node The Node as HstNode
         * @return Subtree size as int
         */
        uint32_t subtreeSize(HstNode node);

        /**
         * Returns the Depth of a given Node
         *
         * @param node The Node as HstNode
         * @return The Depth as uint32_t
         */
        uint32_t depth(HstNode node);

        /**
         * Returns the Depth of a given Node
         *
         * @param node The Node as HstNode
         * @return The Depth as uint32_t
         */
        uint32_t height(HstNode node);

        /**
         * Returns the leftmost leaf of a given Node
         *
         * @param node The Node as HstNode
         * @return The leftmost leaf as HstNode
         */
        HstNode leftmostLeaf(HstNode node);

        /**
         * Returns the rightmost leaf of a given Node
         *
         * @param node The Node as HstNode
         * @return The rightmost leaf as HstNode
         */
        HstNode rightmostLeaf(HstNode node);

        /**
         * Returns the Leaf size of a given Node
         *
         * @param node The Node as HstNode
         * @return the Leaf Size as uint32_t
         */
        uint32_t leafSize(HstNode node);

        /**
         * Returns the Leaf Rank of a given Node
         * Leaf Rank is the amount of Leaves coming before the Node, in node order
         *
         * @param node The Node as HstNode
         * @return the Leaf Size as uint32_t
         */
         uint32_t leafRank(HstNode node);

        /**
         * TODO: Unfinished
         *
         * @param level
         * @param node
         * @return
         */
        HstNode levelAncestor(HstNode node, uint32_t level);

    private:
        HypersuccinctTree() = default;
        bool huffmanFlag;
        //sizes
        std::vector<bool> size;
        std::vector<bool> microSize;
        std::vector<bool> miniSize;
        //miniTrees
        std::vector<MiniTree> miniTrees;
        std::vector<Bitvector> miniFIDs;
        std::vector<succinct_bv::BitVector> miniFIDsSupport;
        //First Top Tree of FIDs +1
        std::vector<Bitvector> miniFIDTopTree;
        //First Low Tree of FIDs +1
        std::vector<Bitvector> miniFIDLowTree;
        std::vector<Bitvector> miniFIDLowTreeAmount;
        std::vector<Bitvector> miniTypeVectors;
        std::vector<succinct_bv::BitVector> miniTypeVectorsSupport;
        std::vector<Bitvector> miniDummys;
        std::vector<succinct_bv::BitVector> miniDummysSupport;
        //LookupTable
        std::vector<LookupTableEntry> lookupTable;

    };
}

#undef DLL_API
#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H