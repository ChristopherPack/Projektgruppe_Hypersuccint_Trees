//
// Created by User on 25.02.2021.
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
    class Bitvector_Utils {
    public:
        template<class T> static Bitvector createBitVectorforMicroTrees(std::vector<std::shared_ptr<pht::UnorderedTree<T>>> fmMicroTrees) {
            //determine size of Bitvector
            Bitvector res;
            for(std::shared_ptr<pht::UnorderedTree<T>> fmMicroTree : fmMicroTrees) {
                createBitVectorForMicroTree(res,fmMicroTree);
            }
            return res;
        }

        template<class T> static Bitvector createBitVectorForMicroTree(Bitvector& res,std::shared_ptr<pht::UnorderedTree<T>> microTree) {
            int32_t size = microTree->getSize();
            //elias gamma code SIZE
            createEliasGamma<T>(res,size);
            //BP FORM in bitform
            Bitvector bp = microTree->toBalancedParenthesis();
            pht::ListUtils::addAll(res,bp);
        }

        template<class T> static Bitvector createEliasGamma(Bitvector& res, const int32_t size) {
            int32_t logSize = floor((log2(size)));
            for(int i=0; i<logSize;i++) {
                res.push_back(false);
            }
            for(int i =0; i<logSize+1;i++) {
                res.push_back((size>>(logSize-i))%2==1);
            }
        }
    };
}


#endif //PROJEKTSUCCINCTTREES_BITVECTOR_UTILS_H
