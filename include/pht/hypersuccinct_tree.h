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
#include "hypersuccinct_tree_factory.h"

namespace pht {
    struct MicroTree {
        uint32_t size;
        uint32_t index;
        MicroTree(uint32_t size, uint32_t index) : size(size), index(index) {}
    };

    struct MiniTree {
        std::vector<MicroTree> microTrees;
        std::vector<bool> interconnections;
    };

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
     * This class is supposed to generate the full hypersuccinct code for any given tree
     *
     */
    template<class T> class HypersuccinctTree {
        friend class HypersuccinctTreeFactory;
    public:
    private:
        HypersuccinctTree() {

        };
        std::vector<MiniTree> miniTrees;
        std::vector<MicroTree> microTrees;
        std::vector<MicroTreeData> lookupTable;
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
