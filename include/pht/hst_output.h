#ifndef PROJEKTSUCCINCTTREES_HST_OUTPUT_H
#define PROJEKTSUCCINCTTREES_HST_OUTPUT_H

#include "pht/hypersuccinct_tree.h"

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

using namespace std;

extern "C" void __declspec(dllexport) hello();

namespace pht {
    class __declspec(dllexport) HypersuccinctTreeOutput {

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
         * prints a given bitvector into the console
         * @param bitvector the bitvector to be printed
         */
        static void printBitvector(const vector<vector<bool>>& bitvector);

        /**
         * splits the FIDs of hypersuccinct Trees into easily readable parts
         * @param bitvector the FID bitvector
         * @param separator the separator for splitting
         * @return Readable version of FID Bitvector as string
         */
        static string splitFIDs(const vector<bool>& bitvector, const string& separator);

        /**
         * TODO: Needs to be adjusted for every change in HypersuccinctTree
         * writes an entire hypersuccinct tree to a file
         * Add a 0 for all empty Bitvectors
         * @param tree the hypersuccinct tree to be written
         */
        static void writeToFile(HypersuccinctTree &tree);

        /**
         * Reads an entire hypersuccinct tree from a file
         * TODO: Has to conform to WriteToFile encoding
         * TODO: Needs to figure out how to even manipulate a hst object from here (via Factory?)
         * @param path the path to the file as string
         * @return hypersuccinct tree
         */
        static HypersuccinctTree readFromFile(string path);

    private:
        /**
         * Writes a Bitvector to a file in a space efficient way
         *
         * @param file File to write to
         * @param bitvector the bitvector to write
         */
        static void writeBitvectorToFile(std::ofstream &file, Bitvector& bitvector);

        /**
         * Writes Content of a small Bitvector into a larger Bitvector
         * Encodes small Bitvector with Elias Gamma
         * @param bitvector The Bitvector to write
         * @param target The Bitvector to write into
         */
        static void createFileBitvector(Bitvector bitvector, Bitvector& target);

        static void createFileBitvector(std::vector<Bitvector> bitvector, Bitvector& target);

        /**
         * Reads a space efficient Bitvector from a file
         *
         * @param file the file to read from
         * @return the read bitvector
         */
        static Bitvector readBitvectorFromFile(std::ifstream &file);

        /**
         * Duplicates every instance of the given separator in a given Bitvector
         * Helper function for writeToFile
         * TODO: Currently unused
         * @param bitvector the given Bitvector
         * @param separator the separator as string
         * @return the modified Bitvector
         */
        static Bitvector addDuplicateSeparator(const Bitvector& bitvector, const string& separator);
    };
}

#undef DLL_API
#endif //PROJEKTSUCCINCTTREES_HST_OUTPUT_H