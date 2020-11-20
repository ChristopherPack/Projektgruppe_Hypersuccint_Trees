#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_

#include "unordered_tree.h"

#include <iostream>

namespace pht {
    /**
     * An implementation of the farzan-munro-algorithm. 
     * 
     * This class implements the algorithm of Farzan and Munro to decompose a tree 
     * into multiple smaller subtrees with a size between a parameter l and 2l.
     */
    template<class T> class FarzanMunro {
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
        static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> decompose(const std::shared_ptr<pht::UnorderedTree<T>> tree, const uint32_t l) {
            assert(tree != nullptr && "Invalid tree");
            assert(l >= 1 && "l cannot be 0");

            if(tree->isEmpty() || tree->getSize() <= l) {
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> result;
                result.push_back(std::shared_ptr<pht::UnorderedTree<T>>(tree));
                return result;
            } else {
                permanentComponents.clear();
                decompose(tree, tree->getRoot(), l);
                return permanentComponents;
            }
        }

    private:
        inline static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> permanentComponents;

        /**
         * Greedily packs the components and node v into new components with l < getSize() < 2l. 
         * 
         * @param[in] tree A pointer to the original tree to decompose. 
         * @param[in] v A pointer to the node which child-components are getting packed together. 
         * @param[in] components A list with pointers to the components to pack together. 
         * @param[in] l The size of the new components. 
         * @tparam T The type of data stored in the nodes of the tree. 
         */
        static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> greedilyPack(const std::shared_ptr<pht::UnorderedTree<T>> tree, const std::shared_ptr<pht::Node<T>> currentNode, std::vector<std::shared_ptr<pht::UnorderedTree<T>>> oldComponents, const uint32_t idealSize) {
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> newComponents;

            do {
                std::shared_ptr<pht::UnorderedTree<T>> newComponent = std::make_shared<pht::UnorderedTree<T>>();
                newComponent->add(currentNode);

                //Combine components
                while(!oldComponents.empty() && newComponent->getSize()-1 < idealSize) {
                    newComponent->add(oldComponents.at(0), currentNode);
                    oldComponents.erase(oldComponents.begin());
                }

                newComponents.push_back(newComponent);
            } while(!oldComponents.empty());

            if(newComponents.size() == 1 && newComponents.at(0)->getSize() < idealSize) {
                return newComponents;
            } else {
                //TODO Extract into Utils
                permanentComponents.insert(permanentComponents.end(), newComponents.begin(), newComponents.end());
                return std::vector<std::shared_ptr<pht::UnorderedTree<T>>>();
            }
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
        template<class T> static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> decompose(const std::shared_ptr<pht::UnorderedTree<T>> tree, const std::shared_ptr<pht::Node<T>> currentNode, const uint32_t idealSize) {
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> temporaryComponents;
            if(tree->isLeaf(currentNode)) {
                return greedilyPack(tree, currentNode, temporaryComponents, idealSize);
            } else {
                for(std::shared_ptr<pht::Node<T>> child : tree->getDirectDescendants(currentNode)) {
                    //TODO Extract into Utils
                    std::vector<std::shared_ptr<pht::UnorderedTree<T>>> childComponents = decompose(tree, child, idealSize);
                    temporaryComponents.insert(temporaryComponents.end(), childComponents.begin(), childComponents.end());
                }
            }
            
            //TODO Extract into Utils or UnorderedTree
            std::vector<std::shared_ptr<pht::Node<T>>> heavyChildren = tree->getDirectDescendants(currentNode);
            heavyChildren.erase(std::remove_if(heavyChildren.begin(), heavyChildren.end(), [tree, idealSize](std::shared_ptr<pht::Node<T>> child){ return !tree->isHeavy(child, idealSize); }), heavyChildren.end());
            
            if(heavyChildren.size() <= 1) {
                //TODO Extract into Utils
                //auto heavyComponentPosition = std::find_if(temporaryComponents.begin(), temporaryComponents.end(), [tree, heavyChildren](std::shared_ptr<pht::UnorderedTree<T>> component){ return component->getRoot() == heavyChildren.at(0); });
                return greedilyPack(tree, currentNode, temporaryComponents, idealSize);
            } else {
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> group;
                do {
                    group.clear();
                    while(!temporaryComponents.empty() && !tree->isHeavy(temporaryComponents.at(0)->getRoot(), idealSize)) {
                        group.push_back(temporaryComponents.at(0));
                        temporaryComponents.erase(temporaryComponents.begin());
                    }
                    if(!temporaryComponents.empty()) {
                        permanentComponents.push_back(temporaryComponents.at(0));
                        temporaryComponents.erase(temporaryComponents.begin()); //Skip heavy component
                    }
                    std::vector<std::shared_ptr<pht::UnorderedTree<T>>> packedComponents = greedilyPack(tree, currentNode, group, idealSize);
                    permanentComponents.insert(permanentComponents.end(), packedComponents.begin(), packedComponents.end());
                } while(!temporaryComponents.empty());
                return std::vector<std::shared_ptr<pht::UnorderedTree<T>>>();
            }
        }
    };

}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_