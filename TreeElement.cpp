//
// Created by Christopher Pack on 06.11.2020.
//

#include "TreeElement.h"

TreeElement::TreeElement() {
    children = new ArrayList<TreeElement>();
    isRoot = true;
    isHeavy = false;
    isRootOf = new ArrayList<TreeStructure>();
    identifier = ' ';
}

TreeElement::TreeElement(TreeElement ancestor) {
    children = new ArrayList<TreeElement>();
    if(ancestor != null)
        this->ancestor = ancestor;
    else {
        isRoot = true;
    }
    isHeavy = false;
    isRootOf = new ArrayList<TreeStructure>();
    identifier = ' ';
}

/**
 * Adds a child of Class TreeElement
 * @param child TreeElement object to add
 */
void TreeElement::addChild(TreeElement child) {
    if(!children.contains(child)) {
        children.add(child);
    }
}

/**
 * Adds a set of children
 * @param children ArrayList of TreeElements to add
 */
void TreeElement::addChildren(std::vector<TreeElement> children) {
    this.children.addAll(children);
}

/**
 * Removes a child from children
 * @param child child to remove
 */
void TreeElement::removeChild(TreeElement child) {
    children.remove(child);
}