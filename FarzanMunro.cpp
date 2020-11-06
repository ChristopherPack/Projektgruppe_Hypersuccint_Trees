//
// Created by Christopher Pack on 06.11.2020.
//

#include "FarzanMunro.h"
/**
 * So this is supposed to do decompose
 * For now it does that with TreeStructure would like to use a Java internal tree later if such a thing exists
 * @param root original Tree as TreeStructure
 * @param size size limiter of created subtrees
 * @return ArrayList of created SubTrees
 * todo: CPP PLEASE
 */
void FarzanMunro::decompose(TreeElement root, int size) {
    //Decompose creates TreeStructures for every Leaf in TreeElement in base and makes them temporary
    //even if TreeStructure holds a list of all elements, we still follow FR to the letter for now

    ArrayList<TreeStructure> components = new ArrayList<>();
    if(root.getChildren().isEmpty()) {
        TreeStructure base = new TreeStructure(root);
        components.add(base);
        root.addIsRootOf(base);
    } else {
        //Recursive Call to decompose all children of root
        for(TreeElement child: root.getChildren()) {
            components.addAll(decompose(child, size));
        }
    }
    //need to add all heavy children into a List for checking
    ArrayList<TreeElement> heavyChildren = new ArrayList<>();
    for(TreeElement child : root.getChildren())
    {
        if(child.isHeavy()) {
            heavyChildren.add(child);
        }
    }
    ArrayList<TreeElement> cleansedChildren = root.getChildren();
    if(heavyChildren.size()==0) {
        //GreedyPacking(root+children)
        greedilyPack(root,root.getChildren(),components,size);
    }
    else if (heavyChildren.size()==1) {
        root.setHeavy(true);
        if(heavyChildren.get(0).getIsRootOf().get(0).isFinalized()) {
            //GreedyPacking(root+children EXCEPT heavy)
            cleansedChildren.removeAll(heavyChildren);
            greedilyPack(root,cleansedChildren,components,size);
        } else {
            //GreedyPacking(root+children)
            greedilyPack(root,root.getChildren(),components,size);
        }
    }
    else {
        root.setHeavy(true);
        if(cleansedChildren.size()==heavyChildren.size()) {
            TreeStructure addition = new TreeStructure(root);
            root.addIsRootOf(addition);
            addition.setFinalized(true);
            components.add(addition);
        }
        else {
            //Resolve packing of children
            //GreedyPacking(root+children BETWEEN HEAVIES)
            while(!cleansedChildren.isEmpty()) {
                //this is supposed to split the TreeElements properly
                ArrayList<TreeElement> partChild = new ArrayList<>();
                while(!cleansedChildren.get(0).equals(heavyChildren.get(0))) {
                    //adds child to the arrayList
                    partChild.add(cleansedChildren.get(0));
                    cleansedChildren.remove(cleansedChildren.get(0));
                }
                //removes child from heavy and cleansed
                cleansedChildren.remove(0);
                heavyChildren.remove(0);
                greedilyPack(root, partChild, components, size);
                if(heavyChildren.isEmpty()) {
                    greedilyPack(root,cleansedChildren,components,size);
                    break;
                }
            }
        }

    }
    return components;
}
/**
 * Implements greedily Pack algorithm
 * @param root root for packing
 * @param subElements children of root for packing
 * @param size size limiter of created subtrees
 * @param components
 * todo: CPP PLEASE
 */
void FarzanMunro::greedilyPack(TreeElement root, std::vector<TreeElement> subElements, std::vector<TreeStructure> components, int size) {
    ArrayList<TreeStructure> subTreesOfElements = new ArrayList<>();
    for(TreeElement elem:subElements) {
        if(elem.getIsRootOf()!=null) {
            subTreesOfElements.add(elem.getIsRootOf().get(0));
        }
    }

    //subTrees will be added to components
    ArrayList<TreeStructure> subTrees = new ArrayList<>();
    int s = 0;
    int m = 0;
    int limit = size;
    //works through all element components and merges them
    //also removes unmerged subTrees from components and adds the merged subTrees
    while(!subTreesOfElements.isEmpty()) {
        for(TreeStructure sub : subTreesOfElements) {
            limit = limit-sub.getElements().size();
            m++;
            if(limit==0) {
                break;
            }
        }
        TreeStructure subTree = new TreeStructure(root);
        for(int i = s; i<m; i++) {
            TreeStructure merger = subTreesOfElements.get(s);
            subTree.addChildren(root, merger.getElements());
            subTreesOfElements.remove(merger);
            components.remove(merger);
        }
        m=0;
        //todo: could maybe just add subTree to components
        subTrees.add(subTree);
        root.addIsRootOf(subTree);
        //todo: very weird equals call
        if(subTree.getElements().size()>=size || subTree.getElements().equals(subElements)) {
            subTree.setFinalized(true);
            root.setHeavy(true);
        }
    }
    components.addAll(subTrees);
}
}
