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
            ELIAS_GAMMA, VECTOR_INDEX, STATIC
        };

        struct IndexingInformation {
            uint32_t multiplier;
            uint32_t staticSize;
            Bitvector::const_iterator& indexStart;
            const Bitvector::const_iterator& indexEnd;
        };

        static Bitvector::const_iterator nullIterator;

        static uint32_t encodeNumber(Bitvector& bitvector, uint32_t num, NumberEncoding encoding);

        static uint32_t encodeNumber(std::insert_iterator<Bitvector> iterator, uint32_t num, NumberEncoding encoding);

        static uint32_t decodeNumber(const Bitvector& bitvector, NumberEncoding encoding);

        static uint32_t decodeNumber(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, NumberEncoding encoding);

        static Bitvector getEntry(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information);

        static uint32_t getEntryCount(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information);

        /**
         * Converts a string of type 0100110 into a bitvector
         *
         * @param input
         * @return bitvector
         */
        static Bitvector convertToBitvector(const std::string& input);

    private:
        static uint32_t decodeEliasGamma(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end);

        static uint32_t decodeBinary(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end);

        static uint32_t encodeBinary(std::insert_iterator<Bitvector>& iterator, uint32_t num);

        static uint32_t encodeEliasGamma(std::insert_iterator<Bitvector>& iterator, uint32_t num);

        static Bitvector getEntryAtEliasGamma(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t multiplier);

        static Bitvector getEntryAtVectorIndex(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd);

        static uint32_t countOccurences(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, bool countZeros = false);

        static Bitvector getEntryAtStatic(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t size);

        static uint32_t getEntryCountEliasGamma(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t multiplier);

        static uint32_t getEntryCountVectorIndex(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd);

        static uint32_t getEntryCountStatic(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t size);
    };
}


#endif //PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H