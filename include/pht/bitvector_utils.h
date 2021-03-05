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
         * @tparam T Class implemented in UnorderedTree Todo: Check usage
         * @param fmMicroTrees List of UnorderedTrees (MicroTrees)
         * @return std::vector<bool>
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
         * This Class modifies a Bitvector with a given UnorderedTrees
         * It is supposed to use a MicroTree from the Farzan Munro Algorithm
         *
         * @tparam T Class implemented in UnorderedTree Todo: Check usage
         * @param vector The Bitvector to be modified
         * @param microTree The UnorderedTree (MicroTree)
         * @return std::vector<bool>    todo: check return necessity
         */
        template<class T> static Bitvector createBitVectorForMicroTree(Bitvector& vector,std::shared_ptr<pht::UnorderedTree<T>> microTree) {
            int32_t size = microTree->getSize();
            //elias gamma code SIZE
            createEliasGamma<T>(vector,size);
            //BP FORM in bitform
            Bitvector bp = microTree->toBalancedParenthesis();
            pht::ListUtils::addAll(vector,bp);
        }

        /**
         * This class creates EliasGamma Code for a given Number and adds it to a Bitvector
         * It is supposed to encode the size of a tree from the Farzan Munro Algorithm
         *
         * @tparam T Class implemented in UnorderedTree Todo: Check usage
         * @param vector The Bitvector to be modified
         * @param size The size to encode
         * @return std::vector<bool>    todo: check return necessity
         */
        template<class T> static Bitvector createEliasGamma(Bitvector& vector, const uint32_t size) {
            int32_t logSize = floor((log2(size)));
            for(int i=0; i<logSize;i++) {
                vector.push_back(false);
            }
            for(int i =0; i<logSize+1;i++) {
                vector.push_back((size>>(logSize-i))%2==1);
            }
        }
    };
}


#endif //PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
