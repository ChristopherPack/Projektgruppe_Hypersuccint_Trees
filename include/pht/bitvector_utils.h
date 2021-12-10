#ifndef PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
#define PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H

#include <iostream>
#include <iterator>
#include <set>
#include <vector>

#include "unordered_tree.h"
#include "list_utils.h"

class BitvectorUtilsTest_getEntyAtHuffmanTest_Test;
class BitvectorUtilsTest_getEntryCountTest_Test;

namespace pht{
    typedef std::vector<bool> Bitvector;
    /**
     * This class provides functionality for the std::vector<bool> construct
     * which is not implemented or very verbose in the standard library.
     * Functionality concerns specific encodings for the implemented Farzan Munro Algorithm.
     */
    class __declspec(dllexport) BitvectorUtils {
        friend class ::BitvectorUtilsTest_getEntyAtHuffmanTest_Test; //HACK
        friend class ::BitvectorUtilsTest_getEntryCountTest_Test; //HACK
        
    private:
        struct __declspec(dllexport) HuffmanComparator {
            bool operator()(const Bitvector &a, const Bitvector &b)const;
        };

    public:
        /**
         * This struct holds various information used during decoding of specific bitvectors.
         */
        struct __declspec(dllexport) IndexingInformation {
            Bitvector::const_iterator& indexStart; //TODO Documentation
            const Bitvector::const_iterator& indexEnd = nullIterator(); //TODO Documentation
            uint32_t multiplier = 0; //TODO Documentation
            uint32_t staticSize = 0; //TODO
            const std::set<Bitvector, HuffmanComparator>& huffmanTable = {}; //TODO Documentation
        };

        /**
         * This enum defines possible encodings for numbers which BitvectorUtils is able to encode into.
         */
        enum class __declspec(dllexport) NumberEncoding {
            BINARY, ///A normal binary encoding
            ELIAS_GAMMA ///Elias-Gamma encoding, consisting of the length of the number - 1 in unary (as zeros) and the number in binary 
        };

        /**
         * This enum defines possible encodings for numbers which BitvectorUtils is able to decode.
         */
        enum class __declspec(dllexport) BitvectorEncoding {
            ELIAS_GAMMA, //TODO Documentation
            VECTOR_INDEX, //TODO Documentation
            STATIC, //TODO Documentation
            HUFFMAN, //TODO Documentation
            PURE_ELIAS_GAMMA, //TODO Documentation
            STATIC_MATRIX_COLUMN //TODO Documentation
        };

        /**
         * This method returns a dummy iterator to use as unused argument in IndexingInformation.
         */
        static Bitvector::const_iterator& nullIterator() {
            static Bitvector::const_iterator iter = Bitvector::const_iterator();
            return iter;
        }

        /**
         * Encodes the given number with the given encoding. 
         * @param bitvector The bitvector to insert the encoded number into. 
         * @param num The number to encode. 
         * @param encoding The encoding to use. 
         * @return uint32_t The length of the encoded number inserted into the bitvector. 
         */
        static uint32_t encodeNumber(Bitvector& bitvector, uint32_t num, NumberEncoding encoding);

        /**
         * A version of encodeNumber which encodes the number in binary and returns the result instead of inserting it.
         * @param num The number to encode. 
         * @return uint32_t The resulting binary encoding bitvector. 
         */
        static Bitvector encodeNumberReturn(uint32_t num);

        /**
         * Encodes a number with the given encoding into a bitvector. 
         * @param iterator The starting position in the bitvector. 
         * @param num The number to encode. 
         * @param encoding The encoding to use. 
         * @return uint32_t The length of the encoded number inserted into the bitvector. 
         */
        static uint32_t encodeNumber(std::insert_iterator<Bitvector> iterator, uint32_t num, NumberEncoding encoding);

        /**
         * Decodes a number in the give encoding from the COMPLETE given bitvector. 
         * @param bitvector The bitvector from which to decode. 
         * @param encoding The encoding to use. 
         * @return uint32_t The resulting number. 
         */
        static uint32_t decodeNumber(const Bitvector& bitvector, NumberEncoding encoding);

        /**
         * Decodes a number in the give encoding starting from at the given iterator. 
         * @param iterator The starting position in the bitvector. 
         * @param end The ending position in the bitvector. 
         * @param encoding The encoding to use. 
         * @return uint32_t The resulting number. 
         */
        static uint32_t decodeNumber(Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, NumberEncoding encoding);

        /**
         * Extracts the specified entry from the given bitvector. 
         * @param iterator The position from which to start counting entries. 
         * @param offset The index of the entry to extract, starting at iterator. 
         * @param end The end of the bitvector. 
         * @param encoding The encoding of the entry. 
         * @param information Other information needed during the decoding. 
         * @return Bitvector The entry.  
         */
        static Bitvector getEntry(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information);

        /**
         * Calculates the amount of entries in the bitvector, comparable to std::vector.size(). 
         * @param iterator  The position from which to start counting entries. 
         * @param end The end of the bitvector. 
         * @param encoding The encoding of the entry. 
         * @param information Other information needed during the decoding. 
         * @return uint32_t The amount of entries. 
         */
        static uint32_t getEntryCount(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, BitvectorEncoding encoding, IndexingInformation information);

        /**
         * Finds all occurrences of a given pattern in a bitvector
         * Does not identify concatenated patterns (ie 10101 only matches once at the beginning for pattern 101)
         *
         * @param iterator Begin of the bitvector to be matched
         * @param end End of the bitvector to be matched
         * @param patternString the pattern as String (ie 00101)
         * @return Vector of Pairs of Iterators marking begin and end of the pattern
         */
        //TODO Generalize with Encoding if necessary
        static std::vector<std::pair<Bitvector::const_iterator,Bitvector::const_iterator>> findMatches(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const std::string& patternString);

        /**
         * Checks if a bitvector starts with the given pattern. 
         * @param iterator The start of a possible match. 
         * @param end The end of the bitvector. 
         * @param patternBitvector The pattern to find. 
         * @return true If the bitvector starts with the given pattern. 
         * @return false If the bitvector does not start with the given pattern. 
         */
        static bool findBeginningMatch(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const Bitvector& patternBitvector);

        /**
         * Converts a string of type 0100110 into a bitvector. 
         * @param input The (ascii) 0s-and-1s-string to convert. 
         * @return bitvector The binary result. 
         */
        static Bitvector convertToBitvector(const std::string& input);

        /**
         * Counts the occurences of (un-)set bits in the bitvector. 
         * @param iterator The position in the bitvector from which to start counting. 
         * @param end The position in the bitvector where the counting should stop. 
         * @param countZeros Set to true if the unset bits should be counted instead of the set bits. 
         * @return uint32_t The amount of (un-)set bits. 
         */
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

        static Bitvector getEntryAtStaticMatrixColumn(Bitvector::const_iterator& iterator, uint32_t offset, const Bitvector::const_iterator& end, uint32_t size);

        static uint32_t getEntryCountEliasGamma(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t multiplier);

        static uint32_t getEntryCountVectorIndex(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, const Bitvector::const_iterator& indexStart, const Bitvector::const_iterator& indexEnd);

        static uint32_t getEntryCountStatic(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, uint32_t size);

        static uint32_t getEntryCountHuffman(const Bitvector::const_iterator& iterator, const Bitvector::const_iterator& end, std::set<Bitvector, HuffmanComparator> huffmanCodes);
    };
}

#undef DLL_API
#endif //PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H