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

namespace pht {
    /*struct MicroTree {
        uint32_t size;
        uint32_t index;
        MicroTree(uint32_t size, uint32_t index) : size(size), index(index) {}
    };*/

    /**
     * MiniTrees consist of:
     * a Bitvector of encoded MicroTrees
     * a Bitvector of Fully Indexable Dictionaries for MicroTrees
     * a Bitvector that determines specific Connections between MicroTrees
     * a Bitvector that indicates added Dummy Nodes to MicroTrees
     */
    struct MiniTree {
        std::vector<bool> microTrees;
        std::vector<bool> FIDs;
        std::vector<bool> typeVectors;
        std::vector<bool> dummys;
    };

    /**
     * MicroTreeData is a table of generic MicroTree structures, indexed by the structures' Balanced Parenthesis form.
     * The table contains a lot of information about these structures
     * todo: add information
     * todo: check Struct for optimisation (bitvectors?)
     */
    struct MicroTreeData {
        std::vector<bool> bp;
        MicroTreeData(std::vector<bool> bp) : bp(bp) {}
        bool operator==(const MicroTreeData& mtd) const {
            return bp == mtd.bp;
        }
    };


    /**
     * todo: need references in example_service
     * todo: need to check makefiles if everything still works
     * This class is supposed to represent the full hypersuccinct code for any given tree
     * It contains:
     * A List of MiniTrees which contain information about their contained MicroTrees.
     * Bitvectors to represent interconnections between MiniTrees
     * A lookup table for information regarding specific MicroTree structures.
     * todo: MiniTrees could be condensed into a single Bitvector
     * todo: As a result, it might be possible to represent an entire tree by a SINGLE bitvector, making this class useless
     */
    template<class T> class HypersuccinctTree {
        friend class HypersuccinctTreeFactory;
    public:
    //private:
        HypersuccinctTree() {

        };
        std::vector<bool> MicroSize;
        std::vector<bool> MiniSize;
        std::vector<MiniTree> miniTrees;
        std::vector<bool> FIDs;
        std::vector<bool> typeVectors;
        std::vector<bool> dummys;
        std::vector<MicroTreeData> lookupTable;
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
