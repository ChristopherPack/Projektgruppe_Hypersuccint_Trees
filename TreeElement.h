//
// Created by Christopher Pack on 06.11.2020.
//

#ifndef PROJEKTSUCCINCTTREES_TREEELEMENT_H
#define PROJEKTSUCCINCTTREES_TREEELEMENT_H

#include <vector>
#include "TreeStructure.h"

class TreeElement {
public:
    TreeElement ancestor;
    std::vector<TreeElement> children;
    bool isRoot;
    //todo: should be String
    char identifier;

    bool isHeavy;
    std::vector<TreeStructure> isRootOf;

    TreeElement();
    TreeElement(TreeElement ancestor);
    void addChild(TreeElement child);
    void addChildren(std::vector<TreeElement> children);
    void removeChild(TreeElement child);
};


#endif //PROJEKTSUCCINCTTREES_TREEELEMENT_H
