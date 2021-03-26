#ifndef PROJEKTSUCCINCTTREES_BITVECTOR_UTILS
#define PROJEKTSUCCINCTTREES_BITVECTOR_UTILS

#include <cassert>
#include <cstdint>
#include <vector>

namespace pht {
    /**
     * Provides bitvector utility functions.
     *
     * This class provides functionality for the std::vector<bool> construct,
     * which is not implemented or very verbose in the standard library.
     */
    class BitvectorUtils {
    public:
        static void encodeEliasGamma(std::vector<bool>& vector, uint32_t number) {
            //assert(number > 0 && "Elias gamma undefined for 0");
            uint32_t logSize = floor((log2(number)));
            for(uint32_t i = 0; i < logSize; i++) {
                vector.push_back(false);
            }
            for(uint32_t i = 0; i < logSize + 1; i++) {
                vector.push_back((number >> (logSize - i)) % 2 == 1);
            }
        }

        static uint32_t decodeEliasGamma(std::vector<bool>::const_iterator& iterator) {
            if(*iterator == false) {
                uint32_t size = 0;
                while(*iterator == false) {
                    size++;
                    iterator++;
                }
                std::vector<bool> num;
                for(uint32_t j = 0; j <= size; j++) {
                    num.push_back(*iterator);
                    iterator++;
                }
                std::vector<bool>::iterator iterator = num.begin();
                return decodeBinary(iterator, num.size());
            }
            iterator++;
            return 1;
        }

        static void encodeBinary(std::vector<bool>& vector, uint32_t num) {
            if(num == 0) {
                vector.push_back(false);
                return;
            }
            uint32_t size = floor(log2(num)) + 1;
            for(uint32_t i = 0; i < size; i++) {
                vector.push_back((num >> (size - 1 - i)) & 1);
            }
        }

        static uint32_t decodeBinary(std::vector<bool>::const_iterator& iterator, uint32_t length) {
            uint32_t res = 0;
            for(uint32_t i = 0; i < length; i++) {
                res <<= 1;
                res = res | (*iterator ? 1 : 0);
                iterator++;
            }
            return res;
        }

        static std::vector<bool> accessEliasGammaBitvector(std::vector<bool>::const_iterator& iterator, uint32_t index, uint32_t multiplier = 1) {
            assert(multiplier > 0 && "Muliplier cannot be 0");
            for(uint32_t i = 0; i < index; i++) {
                iterator += decodeEliasGamma(iterator)*multiplier;
            }
            uint32_t length = decodeEliasGamma(iterator)*multiplier;
            std::vector<bool> result;
            for(uint32_t j = 0; j < length; j++) {
                result.push_back(*iterator);
                iterator++;
            }
            return result;
        }

        static std::vector<bool> findInBitvector(std::vector<bool>::const_iterator& iterator, uint32_t index, uint32_t size) {
            assert(size > 0 && "Size cannot be 0");
            iterator += (size * index);
            std::vector<bool> res;
            for(uint32_t j = 0; j < size; j++) {
                res.push_back(*iterator);
                iterator++;
            }
            return res;
        }

        static uint32_t countBitOccurrences(std::vector<bool>::const_iterator& start, std::vector<bool>::const_iterator& end, bool countZeros = false) {
            uint32_t occurrences = 0;
            while(start != end) {
                occurrences += (countZeros && !(*start)) || (!countZeros && *start) ? 1 : 0;
                start++;
            }
            return occurrences;
        }

        /**
         * Converts a string of type 0100110 into a bitvector
         *
         * @param input
         * @return bitvector
         */
        static Bitvector convertToBitvector(const std::string& input){
            Bitvector result;

            for(char i : input){
                result.push_back(i=='1');
            }

            return result;
        }
    };
}

#endif//PROJEKTSUCCINCTTREES_BITVECTOR_UTILS