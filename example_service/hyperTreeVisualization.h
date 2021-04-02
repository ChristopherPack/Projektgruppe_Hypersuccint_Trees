#ifndef PROJEKTSUCCINCTTREES_HYPERTREEVISUALIZATION_H
#define PROJEKTSUCCINCTTREES_HYPERTREEVISUALIZATION_H

#include "pht/hypersuccinct_tree.h"

using namespace std;

namespace pht {
    class HypersuccinctTreeVisualizer {

    public:
        static void printTree(HypersuccinctTree<std::string> &tree);
        static void printBitvector(const vector<bool>& bitvector);
        static string splitFIDs(const vector<bool>& bitvector, const string& separator);
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERTREEVISUALIZATION_H