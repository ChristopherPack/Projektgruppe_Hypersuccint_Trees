#ifndef PROJEKTSUCCINCTTREES_HUFFMAN_H
#define PROJEKTSUCCINCTTREES_HUFFMAN_H

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <queue>
#include <vector>

#include <iostream>

#include "list_utils.h"
#include "unordered_tree.h"

namespace pht {
    template<class T> std::ostream& operator<<(std::ostream& out, std::pair<T,uint32_t> entry) {
        out << "(" << entry.first << "," << entry.second << ")";
        return out;
    }

    template<class T> std::ostream& operator<<(std::ostream& out, std::vector<T> vec) {
        out << "[";
        for(uint32_t i = 0; i < vec.size(); i++) {
            if(i > 0) {
                out << ",";
            }
            out << vec.at(i);
        }
        out << "]";
        return out;
    }

    template<class K, class V> std::ostream& operator<<(std::ostream& out, std::map<K,V> map) {
        out << "{\n";
        bool first = true;
        for(std::pair<K,V> entry : map) {
            if(first) {
                first = false;
            } else {
                out << ",\n";
            }
            out << "\t" << entry.first << ": " << entry.second;
        }
        out << "}" << std::endl;
        return out;
    }

    class Huffman {
    public:
        template<class T> static std::map<T,std::vector<bool>> generateTable(std::map<T,uint32_t>& occurrences) {
            //Fill queue with the occurrences
            auto comparator = [](std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> a, std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> b) { return a->getRoot()->getValue().second > b->getRoot()->getValue().second; };
            std::priority_queue<std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>>, std::vector<std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>>>, decltype(comparator)> queue(comparator);
            for(std::pair<T,uint32_t> entry : occurrences) {
                std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> tree = std::make_shared<pht::UnorderedTree<std::pair<T,uint32_t>>>();
                std::shared_ptr<pht::Node<std::pair<T,uint32_t>>> root = std::make_shared<pht::Node<std::pair<T,uint32_t>>>(entry);
                tree->add(root, nullptr);
                queue.push(tree);
            }

            //Build tree by repeatedly combining the two smallest entries
            while(queue.size() > 1) {
                std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> tree1 = queue.top();
                queue.pop();
                std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> tree2 = queue.top();
                queue.pop();
                std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> newTree = std::make_shared<pht::UnorderedTree<std::pair<T,uint32_t>>>();
                std::shared_ptr<pht::Node<std::pair<T,uint32_t>>> root = std::make_shared<pht::Node<std::pair<T,uint32_t>>>(std::pair<T,uint32_t>(T(),tree1->getRoot()->getValue().second+tree2->getRoot()->getValue().second));
                newTree->add(root, nullptr);
                newTree->add(tree2, newTree->getRoot());
                newTree->add(tree1, newTree->getRoot());
                queue.push(newTree);
            }

            std::cout << *queue.top() << std::endl;

            //Fill huffman table
            std::map<T,std::vector<bool>> table;
            fillTable(queue.top(), queue.top()->getRoot(), table, std::vector<bool>());
            std::cout << table << std::endl;
            return table;
        }

    private:
        template<class T> static void fillTable(std::shared_ptr<pht::UnorderedTree<std::pair<T,uint32_t>>> tree, std::shared_ptr<pht::Node<std::pair<T,uint32_t>>> current, std::map<T,std::vector<bool>>& table, std::vector<bool> code) {
            if(tree->isLeaf(current)) {
                table.insert({current->getValue().first, code});
            } else {
                std::vector<std::shared_ptr<pht::Node<std::pair<T,uint32_t>>>> children = tree->getDirectDescendants(current);
                assert(children.size() == 2);
                fillTable(tree, children.at(0), table, ListUtils::combined(code, {false}));
                fillTable(tree, children.at(1), table, ListUtils::combined(code, {true}));
            }
        }
    };
}

#endif //PROJEKTSUCCINCTTREES_HUFFMAN_H