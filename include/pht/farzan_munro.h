#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_

#include "unordered_tree.h"

namespace pht {
    /**
     * An implementation of the farzan-munro-algorithm. 
     * 
     * This class implements the algorithm of Farzan and Munro to decompose a tree 
     * into multiple smaller subtrees with a size between a parameter l and 2l.
     */
    class FarzanMunro {
    public:
        /**
         * Decomposes a tree. 
         * 
         * This method will decompose the given tree into multiple new and smaller subtrees with a size 
         * between l and 2 l.
         * 
         * @param[in] tree A pointer to the tree to decompose. 
         * @param[in] l The size of the new subtrees. Subtrees will never be larger than 2l and mostly bigger than l. 
         * @tparam T The type of data stored in the nodes of the tree. 
         */
        template<class T> static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> decompose(const std::shared_ptr<pht::UnorderedTree<T>> tree, const uint32_t l) {
            assert(tree != nullptr && "Invalid tree");
            assert(l >= 1 && "l cannot be 0");
            if(tree->isEmpty()) {
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> result;
                result.push_back(std::shared_ptr<pht::UnorderedTree<T>>(tree));
                return result;
            }
            return decompose(tree, tree->getRoot(), l);
        }

    private:
        /**
         * Greedily packs the components and node v into new components with l < getSize() < 2l. 
         * 
         * @param[in] tree A pointer to the original tree to decompose. 
         * @param[in] v A pointer to the node which child-components are getting packed together. 
         * @param[in] components A list with pointers to the components to pack together. 
         * @param[in] l The size of the new components. 
         * @tparam T The type of data stored in the nodes of the tree. 
         */
        template<class T> static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> greedilyPack(const std::shared_ptr<pht::UnorderedTree<T>> tree, const std::shared_ptr<pht::Node<T>> v, std::vector<std::shared_ptr<pht::UnorderedTree<T>>> components, const uint32_t l) {
            return std::vector<std::shared_ptr<pht::UnorderedTree<T>>>();
        }

        /**
         * Decomposes a tree. 
         * 
         * This method will decompose the given (sub-)tree into multiple new and smaller subtrees with a size 
         * between l and 2 l. 
         * 
         * @param[in] tree A pointer to the tree to decompose. 
         * @param[in] v A pointer to the current root node of the subtree to decompose. 
         * @param[in] l The size of the new subtrees. Subtrees will never be larger than 2l and mostly bigger than l. 
         * @tparam T The type of data stored in the nodes of the tree. 
         */
        template<class T> static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> decompose(const std::shared_ptr<pht::UnorderedTree<T>> tree, const std::shared_ptr<pht::Node<T>> v, const uint32_t l) {
            return std::vector<std::shared_ptr<pht::UnorderedTree<T>>>();
        }
    };
}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_