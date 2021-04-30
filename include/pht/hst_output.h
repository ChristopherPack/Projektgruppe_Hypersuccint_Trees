#ifndef PROJEKTSUCCINCTTREES_HST_OUTPUT_H
#define PROJEKTSUCCINCTTREES_HST_OUTPUT_H

#include "pht/hypersuccinct_tree.h"

using namespace std;

namespace pht {
    class HypersuccinctTreeVisualizer {

    public:
        /**
         * prints a given hypersuccinct tree into the console
         * @param tree the hypersuccinct tree to be printed
         */
        static void printTree(HypersuccinctTree &tree);

        /**
         * prints a given bitvector into the console
         * @param bitvector the bitvector to be printed
         */
        static void printBitvector(const vector<bool>& bitvector);

        /**
         * splits the FIDs of hypersuccinct Trees into easily readable parts
         * @param bitvector the FID bitvector
         * @param separator the separator for splitting
         * @return Readable version of FID Bitvector as string
         */
        static string splitFIDs(const vector<bool>& bitvector, const string& separator);

        /**
         * writes an entire hypersuccinct tree to a file
         * @param tree the hypersuccinct tree to be written
         */
        static void writeToFile(HypersuccinctTree &tree);

        /**
         * Reads an entire hypersuccinct tree from a file
         * todo: needs to understand and work with markings from writeToFile
         * @param path the path to the file as string
         * @return hypersuccinct tree
         */
        static HypersuccinctTree readFromFile(string path);

    private:
        static void writeBitvector(std::ofstream &file, Bitvector& bitvector);

        /**
         * Duplicates every instance of the given separator in a given Bitvector
         * Helper function for writeToFile
         * @param bitvector the given Bitvector
         * @param separator the separator as string
         * @return the modified Bitvector
         */
        static Bitvector addDuplicateSeparator(const Bitvector& bitvector, const string& separator);
    };
}

#endif //PROJEKTSUCCINCTTREES_HST_OUTPUT_H