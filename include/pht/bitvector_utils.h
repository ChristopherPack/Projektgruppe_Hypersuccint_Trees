//
// Created by Christopher Pack on 25.02.2021.
//

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
        /**
         * This Class creates a Bitvector from a List of given UnorderedTrees
         * It is supposed to use a List of MicroTrees from the Farzan Munro Algorithm
         * Encoding consists of Elias Gamma Code of the size of the MicroTrees and their structure in Balanced Parenthesis form.
         *
         * @tparam T Class implemented in UnorderedTree
         * @param fmMicroTrees List of UnorderedTrees (MicroTrees)
         */
        template<class T> static Bitvector createBitVectorforMicroTrees(std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees) {
            //determine size of Bitvector
            Bitvector res;
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                createBitVectorForMicroTree(res,fmMicroTree);
            }
            return res;
        }

        /**
         * This Class modifies a Bitvector with a given UnorderedTree
         * It is supposed to use a MicroTree from the Farzan Munro Algorithm
         *
         * @tparam T Class implemented in UnorderedTree
         * @param vector The Bitvector to be modified
         * @param microTree The UnorderedTree (MicroTree)
         */
        template<class T> static void createBitVectorForMicroTree(Bitvector& vector,std::shared_ptr<pht::UnorderedTree<T>> microTree) {
            int32_t size = microTree->getSize();
            //elias gamma code SIZE
            createEliasGamma(vector,size);
            //BP FORM in bitform
            Bitvector bp = microTree->toBalancedParenthesis();
            pht::ListUtils::addAll(vector,bp);
        }

        /**
         * This class creates EliasGamma Code for a given Number and adds it to a Bitvector
         * It is supposed to encode the size of a tree from the Farzan Munro Algorithm
         *
         * @param vector The Bitvector to be modified
         * @param size The size to encode
         */
        static void createEliasGamma(Bitvector& vector, const uint32_t size) {
            int32_t logSize = floor((log2(size)));
            for(int i=0; i<logSize;i++) {
                vector.push_back(false);
            }
            for(int i =0; i<logSize+1;i++) {
                vector.push_back((size>>(logSize-i))%2==1);
            }
        }

        /**
         * Decodes Elias Gamma Code into an integer
         * This function uses an integer an is supposed to return the amount of bits that are important after the Elias Gamma Code in the Bitvector while moving the iterator to the begin of these important bits.
         *
         * @tparam I template for Iterator functionality
         * @param iterator Iterator over a Bitvector with Elias Gamma code, pointing at the start of the Elias Gamma Code
         * @return Integer of decoded Elias Gamma
         */
        template<class I> static uint32_t decodeEliasGamma(I& iterator) {
            if(*iterator == false) {
                uint32_t size = 0;
                while(*iterator == false) {
                    size++;
                    iterator++;
                }
                Bitvector num;
                for(int j =0; j<=size; j++) {
                    num.push_back(*iterator);
                    iterator++;
                }
                return bitvectorToNumber(num);
            }
            iterator++;
            return 1;
        }

        /**
         * Converts at number to a bitvector
         *
         * @param num the number as integer
         * @return vector<bool> representing the number
         */
        static Bitvector numberToBitvector(uint32_t num) {
            Bitvector res;
            if (num == 0) {
                res.push_back(false);
                return res;
            }
            int size = floor(log2(num)) + 1;
            for(uint32_t i = 0; i<size; i++) {
                res.push_back((num>>(size-1-i))&1);
            }
            return res;
        }

        /**
         * Converts a bitvector to a Number
         *
         * @param bitvector the bitvector to convert
         * @return the number represented by the bitvector
         */
        static uint32_t bitvectorToNumber(Bitvector bitvector) {
            uint32_t res = 0;
            for(uint32_t i = 0; i<bitvector.size(); i++) {
                res <<= 1;
                res = res | (bitvector.at(i)?1:0);
            }
            return res;
        }

        /**
         * Finds a bitvector from a bitvector indexed by Elias Gamma Code
         * The Bitvector can use multiplied Elias Gamma codes for indexing
         *
         * @param bitvector the indexed bitvector
         * @param index the index as integer
         * @param multiplier the index multiplier as integer
         * @return the bitvector at given index
         */
        static Bitvector getBitvectorAtIndexEG(Bitvector& bitvector, uint32_t index, uint32_t multiplier) {
            //iterator
            auto iterator = findEliasGammaIndex(bitvector, index, multiplier);
            //when index is found:
            //decode Elias Gamma
            int length = decodeEliasGamma(iterator)*multiplier;
            //return until Elias Gamma is done / Return this FID
            Bitvector micro;
            for(int j =0; j<length; j++) {
                micro.push_back(*iterator);
                iterator++;
            }
            return micro;
        }

        static Bitvector::iterator findEliasGammaIndex(Bitvector& bitvector, uint32_t index, uint32_t multiplier) {
            auto iterator = bitvector.begin();
            for(int i=0;i<index;i++) {
                //decode Elias Gamma
                int length = decodeEliasGamma(iterator)*multiplier;
                //Count until Elias Gamma is done / skip this FID
                iterator+=length;
            }
            return iterator;
        }

        /**
         * Finds a bitvector from a bitvector indexed by another bitvector
         * second bitvector must be indexed by Elias Gamma
         *
         * @param bitvector the indexed bitvector
         * @param indexvector the bitvector that indexes the primary bitvector
         * @param index the index as integer
         * @return the bitvector at given index
         */
        static Bitvector getBitvectorAtIndexvector(Bitvector bitvector, Bitvector& indexvector, uint32_t index) {
            //iterator
            //skip these TypeVectors
            auto iterator = bitvector.begin();
            for(int i=0; i<index;i++) {
                uint32_t indexLength = findBitvectorLength(indexvector, i);
                iterator+=indexLength;
            }

            //return typeVector
            uint32_t indexLength = findBitvectorLength(indexvector, index);
            Bitvector typeV;
            for(int j =0;j<indexLength; j++) {
                typeV.push_back(*iterator);
                iterator++;
            }
            return typeV;
        }

        /**
         *
         * @param bitvector
         * @param index
         * @return
         */
        static uint32_t findBitvectorLength(Bitvector& bitvector, uint32_t index) {
            Bitvector fid;
            fid = getBitvectorAtIndexEG(bitvector, index, 1);
            int indexLength = 0;
            for(auto && j : fid) {
                if(j) {
                    indexLength++;
                }
            }
            return indexLength;
        }

        /**
         * Finds a bitvector from a bitvector indexed by a static size
         *
         * @param bitvector the indexed bitvector
         * @param index the index as integer
         * @param size the size as integer
         * @return the bitvector at given index
         */
        static Bitvector getBitvectorAtIndexStaticSize(Bitvector bitvector, uint32_t index, uint32_t size) {
            auto iterator = bitvector.begin();
            iterator+=(size*index);
            Bitvector dummy;
            for(int j =0;j<size; j++) {
                dummy.push_back(*iterator);
                iterator++;
            }
            return dummy;
        }
    };
}


#endif //PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
