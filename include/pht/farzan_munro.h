#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_

#include <iostream>
#include <mutex>

#include "unordered_tree.h"
#include "list_utils.h"

namespace pht {
    /**
     * An implementation of the farzan-munro-algorithm. 
     * 
     * This class implements the algorithm of Farzan and Munro to decompose a tree 
     * into multiple smaller subtrees with a size between a parameter L and 2L.
     */
    template<class T> class FarzanMunro {
    public:
        /**
         * Decomposes a tree and sorts it
         *
         * This method will decompose the given tree into multiple new and smaller subtrees with a size
         * between idealSize and 2*idealSize.
         *
         * @param[in] tree A pointer to the tree to decompose.
         * @param[in] idealSize The size of the new subtrees. Subtrees will never be larger than 2*idealSize and mostly bigger than idealSize.
         * @tparam T The type of data stored in the nodes of the tree.
         * @return A list with pointers to the components of the decomposed tree.
         */
        static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> decompose(const std::shared_ptr<pht::UnorderedTree<T>> tree, const uint32_t idealSize) {
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> result = decomposeRaw(tree, idealSize);
            std::vector<std::pair< std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>> enumResult;
            enumResult = ListUtils::mapped<std::shared_ptr<pht::UnorderedTree<T>>, std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>>(result, [&tree](std::shared_ptr<pht::UnorderedTree<T>> tree1){
                std::pair< uint32_t,uint32_t > pair;
                pair.first = tree->enumerate(tree1->getRoot());
                if(tree1->getSize() > 1) {
                    pair.second = tree->enumerate(tree1->getDirectDescendants(tree1->getRoot()).at(0));
                } else {
                    pair.second = 0;
                }
                return std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>(pair,tree1);});
            ListUtils::sort<std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>>(enumResult, [](std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>> treeA, std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>> treeB){
                if(treeA.first.first == treeB.first.first) {
                    return treeA.first.second<treeB.first.second;
                }
                return treeA.first.first<treeB.first.first;});
            result.clear();
            result = ListUtils::mapped<std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>, std::shared_ptr<pht::UnorderedTree<T>>>(enumResult, [](std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>> tree2){return tree2.second;});
            return result;
        }
        
        /**
         * Decomposes a tree and sorts it
         * This version contains mutex settings for multithreading.
         *
         * This method will decompose the given tree into multiple new and smaller subtrees with a size
         * between idealSize and 2*idealSize.
         *
         * @param[in] tree A pointer to the tree to decompose.
         * @param[in] idealSize The size of the new subtrees. Subtrees will never be larger than 2*idealSize and mostly bigger than idealSize.
         * @param allMutex Mutex used during multithreading-phase. 
         * @tparam T The type of data stored in the nodes of the tree.
         * @return A list with pointers to the components of the decomposed tree.
         */
        static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> decomposeMultiThread(const std::shared_ptr<pht::UnorderedTree<T>> tree, const uint32_t idealSize,std::vector<std::mutex>& allMutex) {
            std::unique_lock<std::mutex> lock(allMutex.at(0));
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> result = decomposeRaw(tree, idealSize);
            lock.unlock();
            std::vector<std::pair< std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>> enumResult;
            enumResult = ListUtils::mapped<std::shared_ptr<pht::UnorderedTree<T>>, std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>>(result, [&tree](std::shared_ptr<pht::UnorderedTree<T>> tree1){
                std::pair< uint32_t,uint32_t > pair;
                pair.first = tree->enumerate(tree1->getRoot());
                if(tree1->getSize() > 1) {
                    pair.second = tree->enumerate(tree1->getDirectDescendants(tree1->getRoot()).at(0));
                } else {
                    pair.second = 0;
                }
                return std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>(pair,tree1);});
            ListUtils::sort<std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>>(enumResult, [](std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>> treeA, std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>> treeB){
                if(treeA.first.first == treeB.first.first) {
                    return treeA.first.second<treeB.first.second;
                }
                return treeA.first.first<treeB.first.first;});
            result.clear();
            result = ListUtils::mapped<std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>>, std::shared_ptr<pht::UnorderedTree<T>>>(enumResult, [](std::pair<std::pair< uint32_t,uint32_t >,std::shared_ptr<pht::UnorderedTree<T>>> tree2){return tree2.second;});
            return result;
        }

        /**
         * Decomposes a tree. 
         * 
         * This method will decompose the given tree into multiple new and smaller subtrees with a size 
         * between idealSize and 2*idealSize.
         * 
         * @param[in] tree A pointer to the tree to decompose. 
         * @param[in] idealSize The size of the new subtrees. Subtrees will never be larger than 2*idealSize and mostly bigger than idealSize. 
         * @tparam T The type of data stored in the nodes of the tree. 
         * @return A list with pointers to the components of the decomposed tree. 
         */
        static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> decomposeRaw(const std::shared_ptr<pht::UnorderedTree<T>> tree, const uint32_t idealSize) {
            assert(tree != nullptr && "Invalid tree");
            assert(idealSize >= 1 && "IdealSize cannot be 0");

            if(tree->isEmpty() || tree->getSize() <= idealSize) {
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> result;
                result.push_back(std::shared_ptr<pht::UnorderedTree<T>>(tree));
                return result;
            } else {
                permanentComponents.clear();
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> temporaryComponents = decompose(tree, tree->getRoot(), idealSize);
                ListUtils::combine(permanentComponents, temporaryComponents);
                return permanentComponents;
            }
        }

    private:
        //TODO Remove documentation
        inline static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> permanentComponents; ///The permanent components of the tree which is currently decomposed. 

        //TODO Remove documentation
        /**
         * Greedily packs the components and node v into new components with idealSize < getSize() < 2*idealSize. 
         * 
         * @param[in] tree A pointer to the original tree to decompose. 
         * @param[in] currentNode A pointer to the node which child-components are getting packed together. 
         * @param[in] oldComponents A list with pointers to the components to pack together. 
         * @param[in] idealSize The size of the new components. 
         * @tparam T The type of data stored in the nodes of the tree. 
         * @return A list with pointers to the new packed components. 
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
                ListUtils::combine(permanentComponents, newComponents);
                return std::vector<std::shared_ptr<pht::UnorderedTree<T>>>();
            }
        }

        //TODO Remove documentation
        /**
         * Decomposes a tree. 
         * 
         * This method will decompose the given (sub-)tree into multiple new and smaller subtrees with a size 
         * between idealSize and 2*idealSize. 
         * 
         * @param[in] tree A pointer to the tree to decompose. 
         * @param[in] currentNode A pointer to the current root node of the subtree to decompose. 
         * @param[in] idealSize The size of the new subtrees. Subtrees will never be larger than 2*idealSize and mostly bigger than idealSize. 
         * @tparam T The type of data stored in the nodes of the tree. 
         * @return A list with pointers to the components of the decomposed tree. 
         */
        static std::vector<std::shared_ptr<pht::UnorderedTree<T>>> decompose(const std::shared_ptr<pht::UnorderedTree<T>> tree, const std::shared_ptr<pht::Node<T>> currentNode, const uint32_t idealSize) {
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> temporaryComponents;
            if(tree->isLeaf(currentNode)) {
                return greedilyPack(tree, currentNode, temporaryComponents, idealSize);
            } else {
                for(std::shared_ptr<pht::Node<T>> child : tree->getDirectDescendants(currentNode)) {
                    ListUtils::combine(temporaryComponents, decompose(tree, child, idealSize));
                }
            }

            std::vector<std::shared_ptr<pht::Node<T>>> heavyChildren = tree->getHeavyDirectDescendants(currentNode, idealSize);

            /*std::cout << currentNode->getValue() << std::endl;
            for(std::shared_ptr<pht::UnorderedTree<T>> tree1 : temporaryComponents) {
                std::cout << tree1->toNewickString() << std::endl;
            }
            std::cout << "PERMANENT: " << std::endl;
            for(std::shared_ptr<pht::UnorderedTree<T>> tree1 : permanentComponents) {
                std::cout << tree1->toNewickString() << std::endl;
            }
            for(std::shared_ptr<pht::Node<T>> node : heavyChildren) {
                std::cout << node->getValue() << std::endl;
            }*/
            /*PHT_LOGGER_DEBUG("Farzan") << currentNode->getValue() << std::endl << pht::Logger::endl();
            for(std::shared_ptr<pht::UnorderedTree<T>> tree1 : temporaryComponents) {
                PHT_LOGGER_DEBUG("Farzan") << tree1->toNewickString() << std::endl << pht::Logger::endl();
            }
            PHT_LOGGER_DEBUG("Farzan") << "PERMANENT: " << std::endl << pht::Logger::endl();
            for(std::shared_ptr<pht::UnorderedTree<T>> tree1 : permanentComponents) {
                PHT_LOGGER_DEBUG("Farzan") << tree1->toNewickString() << std::endl << pht::Logger::endl();
            }
            for(std::shared_ptr<pht::Node<T>> node : heavyChildren) {
                PHT_LOGGER_DEBUG("Farzan") << node->getValue() << std::endl << pht::Logger::endl();
            }*/
            
            if(heavyChildren.size() <= 1) {
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
                    if(!group.empty()) {
                        std::vector<std::shared_ptr<pht::UnorderedTree<T>>> packedComponents = greedilyPack(tree,
                                                                                                            currentNode,
                                                                                                            group,
                                                                                                            idealSize);
                        permanentComponents.insert(permanentComponents.end(), packedComponents.begin(),
                                                   packedComponents.end());
                    }
                } while(!temporaryComponents.empty());
                if(heavyChildren.size() == tree->getDirectDescendants(currentNode).size()) {
                    std::shared_ptr<pht::UnorderedTree<T>> component = std::make_shared<pht::UnorderedTree<T>>();
                    component->add(currentNode);
                    permanentComponents.push_back(component);
                }
                return std::vector<std::shared_ptr<pht::UnorderedTree<T>>>();
            }
        }
    };

}

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_FARZANMUNRO_H_