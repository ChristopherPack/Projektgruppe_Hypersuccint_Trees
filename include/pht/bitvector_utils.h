#ifndef PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
#define PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H

#include <iostream>
#include <iterator>
#include <set>
#include <vector>

#include "unordered_tree.h"
#include "list_utils.h"

namespace pht{
    typedef std::vector<bool> Bitvector;
    /**
     * Provides bitvector utility functions.
     *
     * This class provides functionality for the std::vector<bool> construct,
     * which is not implemented or very verbose in the standard library.
     * Functionality concerns specific encodings for the implemented Farzan Munro Algorithm.
     *
     */
    class Bitvector_Utils {
    public:
        enum class NumberEncoding {
            BINARY, ELIAS_GAMMA
        };
        enum class BitvectorEncoding {
            ELIAS_GAMMA, VECTOR_INDEX, STATIC, HUFFMAN, PURE_ELIAS_GAMMA
        };

        struct HuffmanComparator {
            bool operator()(const Bitvector &a, const Bitvector &b)const;
        };

        struct IndexingInformation {
            Bitvector::const_iterator& indexStart;
            const Bitvector::const_iterator& indexEnd = nullIterator;
            uint32_t multiplier = 0;
            uint32_t staticSize = 0;
            const std::set<Bitvector, HuffmanComparator>& huffmanTable = {};
        };

        static Bitvector::const_iterator nullIterator;

        static uint32_t encodeNumber(Bitvector& bitvector, uint32_t num, NumberEncoding encoding);

        static uint32_t encodeNumber(std::insert_iterator<Bitvector> iterator, uint32_t num, NumberEncoding encoding);

        static uint32_t decodeNumber(const Bitvector& bitvector, NumberEncoding encoding);

        static uint32_t decodeNumber(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, NumberEncoding encoding);

        static Bitvector getEntry(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information);

        static uint32_t getEntryCount(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information);

        /**
         * Finds all occurrences of a given pattern in a bitvector
         * Does not identify concatenated patterns (ie 10101 only matches once at the beginning for pattern 101)
         * TODO: Generalize with Encoding if necessary
         *
         * @param iterator Begin of the bitvector to be matched
         * @param end End of the bitvector to be matched
         * @param separator the pattern as String (ie 00101)
         * @return Vector of Pairs of Iterators marking begin and end of the pattern
         */
        static std::vector<std::pair<Bitvector::const_iterator,Bitvector::const_iterator>> findMatches(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const std::string& patternString);

        static bool findBeginningMatch(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const Bitvector& patternBitvector);

        /**
         * Converts a string of type 0100110 into a bitvector
         *
         * @param input
         * @return bitvector
         */
        static Bitvector convertToBitvector(const std::string& input);

        static uint32_t countOccurences(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, bool countZeros = false);

    private:
        static uint32_t decodeEliasGamma(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end);

        static uint32_t decodeBinary(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end);

        static uint32_t encodeBinary(std::insert_iterator<Bitvector>& iterator, uint32_t num);

        static uint32_t encodeEliasGamma(std::insert_iterator<Bitvector>& iterator, uint32_t num);

        static Bitvector getEntryAtEliasGamma(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t multiplier);

        static Bitvector getEntryAtVectorIndex(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd);

        static Bitvector getEntryAtStatic(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t size);

        static Bitvector getEntryAtHuffman(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, std::set<Bitvector, HuffmanComparator> huffmanCodes);

        static Bitvector getEntryAtPureEliasGamma(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end);

        static Bitvector readEliasGamma(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end);

        static uint32_t getEntryCountEliasGamma(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t multiplier);

        static uint32_t getEntryCountVectorIndex(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd);

        static uint32_t getEntryCountStatic(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t size);

        static uint32_t getEntryCountHuffman(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, std::set<Bitvector, HuffmanComparator> huffmanCodes);
    };
}


#endif //PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H