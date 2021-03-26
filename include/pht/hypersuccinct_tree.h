#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H

namespace pht {
    class HypersuccinctTree {
        friend class HypersuccinctTreeFactory;

    public:
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
         */
        struct MicroTreeData {
            std::vector<bool> bp;

            MicroTreeData(std::vector<bool> bp) : bp(std::move(bp)) {}

            bool operator==(const MicroTreeData& mtd) const {
                return bp == mtd.bp;
            }
        };

    private:
        HypersuccinctTree() {}
        //sizes
        std::vector<bool> microSize;
        std::vector<bool> miniSize;
        //miniTrees
        std::vector<MiniTree> miniTrees;
        std::vector<bool> miniFIDs;
        std::vector<bool> miniTypeVectors;
        std::vector<bool> miniDummys;
        //LookupTable
        std::vector<MicroTreeData> lookupTable;
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_H
