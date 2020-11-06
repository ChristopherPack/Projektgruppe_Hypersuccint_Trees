//
// Created by Christopher Pack on 06.11.2020.
//

#ifndef PROJEKTSUCCINCTTREES_TREESTRUCTURE_H
#define PROJEKTSUCCINCTTREES_TREESTRUCTURE_H


#include "TreeElement.h"
#include <vector>

class TreeStructure {
public:
    TreeElement root;
    std::vector<TreeElement> elements;

    bool finalized;

    TreeStructure(TreeElement root);
    TreeStructure(TreeElement root, std::vector<Treeelement> elements);
    bool addChild(TreeElement ancestor, TreeElement add);
    bool removeElement(TreeElement remove);
    void addChildren(TreeElement ancestor, std::vector<TreeElement> children);
    void addTopElement(TreeElement root, TreeElement set);
};


#endif //PROJEKTSUCCINCTTREES_TREESTRUCTURE_H
