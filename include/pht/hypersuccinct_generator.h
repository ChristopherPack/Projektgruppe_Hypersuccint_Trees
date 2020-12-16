//
// Created by Christopher Pack on 16.12.2020.
//

#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_GENERATOR_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_GENERATOR_H

#include <iostream>

#include "unordered_tree.h"
#include "list_utils.h"
#include "farzan_munro.h"

namespace pht {

    /**
     * todo: need references in example_service
     * todo: need to check makefiles if everything still works
     * This class is supposed to generate the full hypersuccinct code for any given tree
     *
     */
    template<class T>class HypersuccinctGenerator {
    public:
        static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> generateCode(const std::shared_ptr<pht::UnorderedTree<T>> tree) {
            assert(tree != nullptr && "Invalid tree");
            assert(idealSize >= 1 && "IdealSize cannot be 0");

            //generate mini Trees and micro Trees
            //todo: should round up - might need ceil() instead
            int size = round(log(tree->getSize()));
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> miniTrees = pht::FarzanMunro<T>::decompose(tree, size);
            size = round((log(tree->getSize()))/8);
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> microTrees;
            for(UnorderedTree<T> t : miniTrees) {
                ListUtils::addAll(microTrees, pht::FarzanMunro<T>::decompose(t,size));
            }
            //todo: make look up Table for mirco Trees

        }
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_GENERATOR_H
