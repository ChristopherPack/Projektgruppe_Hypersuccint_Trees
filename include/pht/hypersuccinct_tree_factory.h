//
// Created by User on 15.01.2021.
//

#ifndef PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H
#define PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H

#include <iostream>
#include <iterator>
#include <set>

#include "hypersuccinct_tree.h"

namespace pht {
    class HypersuccinctTreeFactory {
    public:
        template<class T> static HypersuccinctTree<T> create(const std::shared_ptr<UnorderedTree<T>> tree) {
            HypersuccinctTree<T> hypersuccinctTree;

            uint32_t size = ceil(pow(log2(tree->getSize()),2.0));
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> miniTrees = pht::FarzanMunro<T>::decompose(tree, size);
            size = ceil((log2(tree->getSize()))/8);
            std::vector<std::shared_ptr<pht::UnorderedTree<T>>> rawMicroTrees;
            std::map<std::shared_ptr<pht::UnorderedTree<T>>, std::vector<std::shared_ptr<pht::UnorderedTree<T>>>> miniToMicro;
            for(std::shared_ptr<pht::UnorderedTree<T>> t : miniTrees) {
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> res = pht::FarzanMunro<T>::decompose(t,size);
                ListUtils::addAll(rawMicroTrees, res);
                miniToMicro.insert({t,res});
            }


            std::map<std::shared_ptr<pht::UnorderedTree<T>>,MicroTree> microToBP;
            for(int i = 0; i < rawMicroTrees.size(); i++) {
                std::vector<bool> bp = rawMicroTrees.at(i)->toBalancedParenthesis();
                hypersuccinctTree.lookupTable.push_back(bp);
                MicroTree mt = MicroTree(rawMicroTrees.at(i)->getSize(), i);
                hypersuccinctTree.microTrees.push_back(mt);
                microToBP.insert({rawMicroTrees.at(i),mt});
            }

            for(int i=0; i<miniTrees.size(); i++) {
                std::vector<std::shared_ptr<pht::UnorderedTree<T>>> micros = miniToMicro.at(miniTrees.at(i));
                std::vector<MicroTree> microTrees2;
                for(std::shared_ptr<UnorderedTree<T>> micro : micros) {
                    microTrees2.push_back(microToBP.at(micro));
                }
                MiniTree mt = MiniTree(microTrees2, std::vector<bool>());
                hypersuccinctTree.miniTrees.push_back(mt);
            }


            //COUTS
            std::cout << hypersuccinctTree.lookupTable.size() << std::endl;

            std::cout << *rawMicroTrees.at(123) << std::endl;
            std::cout << hypersuccinctTree.microTrees.at(123).index << std::endl;
            std::vector<bool> data = (*std::next(hypersuccinctTree.lookupTable.begin(), hypersuccinctTree.microTrees.at(123).index)).bp;
            for(bool d: data) {
                std::cout << d << ", ";
            }
            std::cout << std::endl;
            for(MicroTreeData data2 : hypersuccinctTree.lookupTable)
            {
                for(bool d : data2.bp) {
                    std::cout << d << ",";
                }
                std::cout << std::endl;
            }
            for(MiniTree mi : hypersuccinctTree.miniTrees) {
                std::cout << mi.microTrees.size() << std::endl;
            }
            return hypersuccinctTree;
        }
    private:
    };
}

#endif //PROJEKTSUCCINCTTREES_HYPERSUCCINCT_TREE_FACTORY_H