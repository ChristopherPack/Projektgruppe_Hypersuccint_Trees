//
// Created by Christopher Pack on 06.11.2020.
//todo: CPP PLEASE
//

#include "TreeStructure.h"

TreeStructure::TreeStructure(TreeElement root) {
    this->root = root;
    elements = new ArrayList<TreeElement>();
    elements.add(root);
    finalized = false;
}

TreeStructure::TreeStructure(TreeElement root, std::vector<TreeElement> elements) {
    this.root = root;
    this.elements = elements;
    elements.add(root);
    finalized = false;
}

/**
 * Adds an Element to the Tree
 * @param ancestor Ancestor to add Element to
 * @param add TreeElement to add
 * @return if adding was successful
 */
bool TreeStructure::addChild(TreeElement ancestor, TreeElement add) {
    if(elements.contains(ancestor)) {
        ancestor.addChild(add);
        elements.add(add);
        return true;
    }
    return false;
}

/**
 * Removes an Element from the Tree
 * @param remove TreeElement to remove
 * @return if removal was successful
 */
bool TreeStructure::removeElement(TreeElement remove) {
    if(elements.contains(remove)) {
        TreeElement ancestor = remove.getAncestor();
        ancestor.removeChild(remove);
        ancestor.addChildren(remove.getChildren());
        return true;
    }
    return false;
}

void TreeStructure::addChildren(TreeElement ancestor, std::vector <TreeElement> children) {
    if(elements.contains(ancestor)) {
        elements.addAll(children);
    }
}

void TreeStructure::addTopElement(TreeElement root, TreeElement set) {
    if(elements.contains(root)&&root.getAncestor().equals(set)) {
        elements.add(set);
        setRoot(set);
    }
}