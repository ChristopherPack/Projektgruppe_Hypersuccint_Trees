//
// Created by Christopher Pack on 16.12.2020.
//

#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H

#include <iostream>
#include <functional>

#include "unordered_tree.h"
#include "list_utils.h"
#include "farzan_munro.h"
#include "bitvector_utils.h"

namespace pht {
    typedef std::vector<bool> Bitvector;

    /**
     * MiniTrees consist of:
     * a Bitvector of encoded MicroTrees
     * a Bitvector of Fully Indexable Dictionaries for MicroTrees
     * a Bitvector that determines specific Connections between MicroTrees
     * a Bitvector that indicates added Dummy Nodes to MicroTrees
     */
    struct MiniTree {
        Bitvector microTrees;
        Bitvector FIDs;
        Bitvector typeVectors;
        Bitvector dummys;
    };

    /**
     * MicroTreeData is a table of generic MicroTree structures, indexed by the structures' Balanced Parenthesis form.
     * The table contains a lot of information about these structures
     * todo: add information
     * todo: check Struct for optimisation (more bitvectors!)
     */
    struct MicroTreeData {
        Bitvector bp;
        MicroTreeData(Bitvector bp) : bp(bp) {}
        bool operator==(const MicroTreeData& mtd) const {
            return bp == mtd.bp;
        }
    };


    /**
     * This class is supposed to represent the full hypersuccinct code for any given tree
     * It contains:
     * A List of MiniTrees which contain information about their contained MicroTrees.
     * Bitvectors to represent interconnections between MiniTrees
     * A lookup table for information regarding specific MicroTree structures.
     * It implements:
     * Get Functions Only!! No modifying functions!!!
     *
     *
     * todo: MiniTrees could be condensed into a single Bitvector
     * todo: As a result, it might be possible to represent an entire tree by a SINGLE bitvector, making this class useless
     * todo: representing the entire class as one bitvector will be done with Elias Gamma indexing
     *
     *
     * todo: IDEA: When Hypersuccinct Tree is complete, you only need to use this class to navigate the tree (move get functions here)
     */
    template<class T> class HypersuccinctTree {
        friend class HypersuccinctTreeFactory;
    public:

        /**
         * Returns the MicroTree representation from the MicroTree Bitvector at the given index
         *
         * @param bitvector the MicroTree bitvector
         * @param index the index as integer
         * @return the MicroTree in Balanced Parenthesis form as bitvector
         */
        Bitvector getMicroTree(MiniTree& miniTree,uint32_t index) {
            return pht::Bitvector_Utils::findEliasGammaIndex(miniTree.microTrees,index,2);
        }

        /**
         * Returns the MicroTree FID representation from the FID Bitvector at the given index
         *
         * @param bitvector the FID bitvector
         * @param index the index as integer
         * @return the FID as bitvector
         */
        Bitvector getMicroFID(MiniTree& miniTree,uint32_t index) {
            return pht::Bitvector_Utils::findEliasGammaIndex(miniTree.FIDs,index,1);
        }

        /**
         * Returns the MicroTree Typevector representation from the Typevector Bitvector at the given index
         *
         * @param bitvector the Typevector bitvector
         * @param fids the FID bitvector
         * @param index the index as integer
         * @return the Typevector as bitvector
         */
        Bitvector getMicroTypeVector(MiniTree& miniTree , uint32_t index) {
            return pht::Bitvector_Utils::findBitvectorBitIndex(miniTree.typeVectors, miniTree.FIDs, index);
        }

        /**
         * Returns the Dummy Node representation from the Dummy Bitvector at the given index
         *
         * @param bitvector the Dummy bitvector
         * @param index the index as integer
         * @return the Dummy is bitvector
         */
        Bitvector getMicroDummys(MiniTree& miniTree, uint32_t index) {
            uint32_t size = pht::Bitvector_Utils::bitvectorToNumber(microSize);
            uint32_t dummySize = floor(log2(2*size+1))+1;
            return pht::Bitvector_Utils::findStaticSizeIndex(miniTree.dummys, index, dummySize);
        }

        Bitvector getMicroSize() {
            return microSize;
        }

        Bitvector getMiniSize() {
            return miniSize;
        }

        std::vector<MiniTree> getMiniTree() {
            return miniTrees;
        }

        Bitvector getFIDs() {
            return miniFIDs;
        }

        Bitvector getminiTypeVectors() {
            return miniTypeVectors;
        }

        Bitvector getMiniDummys() {
            return miniDummys;
        }

    private:
        HypersuccinctTree() {

        };
        //todo: ORDER
        std::vector<bool> microSize;
        std::vector<bool> miniSize;
        std::vector<MiniTree> miniTrees;
        std::vector<bool> miniFIDs;
        std::vector<bool> miniTypeVectors;
        std::vector<bool> miniDummys;
        std::vector<MicroTreeData> lookupTable;
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
