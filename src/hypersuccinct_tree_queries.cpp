//
// Created by Christopher Pack on 20.05.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>

using namespace pht;

bool HypersuccinctTree::isDummyAncestorWithinMiniTree(HstNode node) {
    //assert(std::get<1>(node) <= Bitvector_Utils::getEntryCount(miniTrees.at(std::get<0>(node)).microTrees.cbegin(),miniTrees.at(std::get<0>(node)).microTrees.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {2, 0, Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator}));
    MiniTree mini = getMiniTree(std::get<0>(node));
    if(mini.rootAncestors.empty()) {
        return false;
    }
    HstNode dummy = {std::get<0>(node), Bitvector_Utils::decodeNumber(mini.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) , Bitvector_Utils::decodeNumber(mini.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY)};
    assert(std::get<2>(dummy) <= (getMicroTree(mini, std::get<1>(dummy)).size()/2));
    if(std::get<2>(node) == 0) {
        //explicitly stored Root is Ancestor
        Bitvector rootAnc = mini.rootAncestors;
        if(rootAnc.at(std::get<1>(node))) {
            return true;
        }
        return false;
    }
    else if(std::get<1>(node) == std::get<1>(dummy)) {
        //lookupTable
        Bitvector index = getMicroTree(mini, std::get<1>(node));
        return lookupTableMatrixComparison(getLookupTableEntry(index), std::get<2>(node),std::get<2>(dummy));
    }
    else {
        //explicitly stored - microTreeDummyNode is Ancestor of MiniDummy
        Bitvector dummyAnc = mini.dummyAncestors;
        if(dummyAnc.at(std::get<1>(node))) {
            return true;
        }
        return false;
    }
    return false;
}

bool HypersuccinctTree::isDummyAncestorWithinMicroTree(HstNode node) {
    MiniTree mini = getMiniTree(std::get<0>(node));
    Bitvector dummy = getMicroDummys(mini,std::get<1>(node));
    uint32_t dummyNum = Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY);
    if(dummyNum == 0) {
        return false;
    }
    return lookupTableMatrixComparison(getLookupTableEntry(getMicroTree(mini,std::get<1>(node))), std::get<2>(node),dummyNum);
}

bool HypersuccinctTree::isAncestor(HstNode node, HstNode anc) {
    if(std::get<0>(node) == std::get<0>(anc))
    {
        if(std::get<1>(node)==std::get<1>(anc)) {
            MiniTree mini = getMiniTree(std::get<0>(anc));
            Bitvector index = getMicroTree(mini,std::get<1>(anc));
            return lookupTableMatrixComparison(getLookupTableEntry(index), std::get<2>(anc),std::get<2>(node));
        }
        else
        {
            if(std::get<1>(node) < std::get<1>(anc)) {
                return false;
            }
            MiniTree mini = getMiniTree(std::get<0>(anc));
            uint32_t ancMicroDummy = Bitvector_Utils::decodeNumber(getMicroDummys(mini, std::get<1>(anc)),Bitvector_Utils::NumberEncoding::BINARY);
            if(std::get<2>(anc) != 0 && ancMicroDummy == 0) {
                return false;
            }
            Bitvector index = getMicroTree(mini,std::get<1>(anc));
            if(!lookupTableMatrixComparison(getLookupTableEntry(index), ancMicroDummy,std::get<2>(anc))) {
                return false;
            }
            return microTreeAncMatrixComparison(mini, std::get<1>(anc), std::get<1>(node));
        }
    }
    else
    {
        if(std::get<0>(node) < std::get<0>(anc)) {
            return false;
        }
        uint32_t ancMiniDummy = Bitvector_Utils::decodeNumber(getMiniDummy(std::get<0>(anc)),Bitvector_Utils::NumberEncoding::BINARY);
        if(std::get<2>(anc) != 0 && ancMiniDummy == 0) {
                return false;
        }
        if(!isDummyAncestorWithinMiniTree(anc) && ancMiniDummy != 0) {
            return false;
        }
        return miniTreeAncMatrixComparison(std::get<0>(anc),std::get<0>(node));
    }
}

uint32_t HypersuccinctTree::degree(HstNode node) {
    if(std::get<2>(node) > 0) {
        //LookupTable
        MiniTree miniTree = getMiniTree(std::get<0>(node));
        if(Bitvector_Utils::decodeNumber(getMicroDummys(miniTree,std::get<1>(node)),Bitvector_Utils::NumberEncoding::BINARY) == std::get<2>(node)) {
            //TODO: Maybe return degree of child? (MicroTree root)
            return 1;
        }
        if(Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == std::get<2>(node) ) {
            //TODO: Maybe return degree of child? (MiniTree root)
            return 1;
        }
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        auto iter = entry.degree.cbegin();
        Bitvector degreeB = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.degree.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        return Bitvector_Utils::decodeNumber(degreeB,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(std::get<1>(node) > 0) {
        Bitvector fid = getFIDforMicroTree(std::get<0>(node), std::get<1>(node));
        return fid.size();
    }
    Bitvector fid = getFIDforMiniTree(std::get<0>(node));
    return fid.size();
}

uint32_t HypersuccinctTree::subtree_size(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<2>(node) > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMicroTree(node)) {
            isDummyAncestor = 1;
            auto dummyIter = miniTree.dummys.cbegin();
            uint32_t dummyIndex = 0;
            for(int i=0; i<std::get<1>(node);i++) {
                Bitvector dummy = getMicroDummys(miniTree, 0);
                if(Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    dummyIndex++;
                }
            }
            auto iter = miniTree.microDummyPointers.cbegin();
            Bitvector subTreeB = Bitvector_Utils::getEntry(iter,dummyIndex,miniTree.microDummyPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
            auto iter2 = miniTree.microSubTrees.cbegin();
            Bitvector subtreeSize = Bitvector_Utils::getEntry(iter2,dummyPointer,miniTree.microSubTrees.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
            res += Bitvector_Utils::decodeNumber(subtreeSize, Bitvector_Utils::NumberEncoding::BINARY);
        }
        if(isDummyAncestorWithinMiniTree(node)) {
            isDummyAncestor = 1;
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            res += Bitvector_Utils::decodeNumber(getMiniTree(dummyPointer).subTree, Bitvector_Utils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        auto iter = entry.subTrees.cbegin();
        Bitvector degreeB = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.subTrees.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        return res + Bitvector_Utils::decodeNumber(degreeB,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(std::get<1>(node) > 0) {
        if(isDummyAncestorWithinMiniTree(node)) {
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            res += Bitvector_Utils::decodeNumber(getMiniTree(dummyPointer).subTree, Bitvector_Utils::NumberEncoding::BINARY);
        }
        auto iter = miniTree.microSubTrees.cbegin();
        Bitvector subTreeB = Bitvector_Utils::getEntry(iter,std::get<1>(node),miniTree.microSubTrees.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        return res + Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.subTree,Bitvector_Utils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::depth(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<2>(node) > 0) {

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        auto iter = entry.nodeDepths.cbegin();
        Bitvector entryDepth = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.nodeDepths.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        res += Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(std::get<1>(node) > 0) {
        auto iter = miniTree.rootDepths.cbegin();
        Bitvector microDepth = Bitvector_Utils::getEntry(iter,std::get<1>(node),miniTree.rootDepths.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        res += Bitvector_Utils::decodeNumber(microDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniDepth, Bitvector_Utils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::height(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<2>(node) > 0) {
        if(isDummyAncestorWithinMicroTree(node) || isDummyAncestorWithinMiniTree(node)) {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
            auto iter = entry.nodeDepths.cbegin();
            Bitvector entryDepth = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.nodeDepths.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
            res -= (Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1);
        }
        else {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
            auto iter = entry.nodeHeights.cbegin();
            Bitvector entryDepth = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.nodeHeights.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
            return res + Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
    }
    if(std::get<1>(node) > 0) {
        if(isDummyAncestorWithinMiniTree({std::get<0>(node),std::get<1>(node),0})) {
            auto iter = miniTree.rootDepths.cbegin();
            Bitvector microDepth = Bitvector_Utils::getEntry(iter,std::get<1>(node),miniTree.rootDepths.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
            res -= (Bitvector_Utils::decodeNumber(microDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1);
        }
        else {
            auto iter = miniTree.rootHeights.cbegin();
            Bitvector microDepth = Bitvector_Utils::getEntry(iter,std::get<1>(node),miniTree.rootHeights.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
            return res + Bitvector_Utils::decodeNumber(microDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniHeight, Bitvector_Utils::NumberEncoding::BINARY);
}

HstNode HypersuccinctTree::leftmost_leaf(HstNode node) {
    uint32_t miniNum = std::get<0>(node);
    uint32_t microNum = std::get<1>(node);
    uint32_t nodeNum = std::get<2>(node);
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    MiniTree leftmostMini = miniTree;
    uint32_t miniDummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t miniDummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);

    if(std::get<2>(node) > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        auto iter = entry.leftmost_leaf.cbegin();
        Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, nodeNum, entry.leftmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        nodeNum = Bitvector_Utils::decodeNumber(entryleftmost, Bitvector_Utils::NumberEncoding::BINARY);
        Bitvector microDummy = getMicroDummys(miniTree, microNum);
        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY));
        }
        else if (nodeNum == Bitvector_Utils::decodeNumber(microDummy, Bitvector_Utils::NumberEncoding::BINARY)) {
            auto dummyIter = miniTree.dummys.cbegin();
            uint32_t dummyIndex = 0;
            for(int i=0; i<std::get<1>(node);i++) {
                Bitvector dummy = getMicroDummys(miniTree, 0);
                if(Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    dummyIndex++;
                }
            }
            auto itermicroPoint = miniTree.microDummyPointers.cbegin();
            Bitvector subTreeB = Bitvector_Utils::getEntry(itermicroPoint, dummyIndex, miniTree.microDummyPointers.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator});
            microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }

    if((std::get<1>(node) > 0 || std::get<2>(node) > 0)  && (microNum != miniDummyTree || nodeNum != miniDummyIndex)) {
        auto iterm = leftmostMini.microTreeLeftmostLeafPointers.cbegin();
        Bitvector leftmostMicro = Bitvector_Utils::getEntry(iterm,microNum,leftmostMini.microTreeLeftmostLeafPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        auto iter = entry.leftmost_leaf.cbegin();
        Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.leftmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        nodeNum = Bitvector_Utils::decodeNumber(entryleftmost, Bitvector_Utils::NumberEncoding::BINARY);

        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY));
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }
    miniNum = Bitvector_Utils::decodeNumber(miniTree.miniTreeLeftmostLeafPointer,Bitvector_Utils::NumberEncoding::BINARY);
    leftmostMini = getMiniTree(miniNum);
    auto iterm = leftmostMini.microTreeLeftmostLeafPointers.cbegin();
    Bitvector leftmostMicro = Bitvector_Utils::getEntry(iterm,0,leftmostMini.microTreeLeftmostLeafPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
    microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini,microNum));
    auto iter = entry.leftmost_leaf.cbegin();
    Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.leftmost_leaf.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator});
    nodeNum = Bitvector_Utils::decodeNumber(entryleftmost,Bitvector_Utils::NumberEncoding::BINARY);
    return {miniNum, microNum, nodeNum};
}

HstNode HypersuccinctTree::rightmost_leaf(HstNode node) {
    uint32_t miniNum = std::get<0>(node);
    uint32_t microNum = std::get<1>(node);
    uint32_t nodeNum = std::get<2>(node);
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    MiniTree leftmostMini = miniTree;
    uint32_t miniDummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t miniDummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);

    if(std::get<2>(node) > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        auto iter = entry.rightmost_leaf.cbegin();
        Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, nodeNum, entry.rightmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        nodeNum = Bitvector_Utils::decodeNumber(entryleftmost, Bitvector_Utils::NumberEncoding::BINARY);
        Bitvector microDummy = getMicroDummys(miniTree, microNum);
        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY));
        }
        else if (nodeNum == Bitvector_Utils::decodeNumber(microDummy, Bitvector_Utils::NumberEncoding::BINARY)) {
            auto dummyIter = miniTree.dummys.cbegin();
            uint32_t dummyIndex = 0;
            for(int i=0; i<std::get<1>(node);i++) {
                Bitvector dummy = getMicroDummys(miniTree, 0);
                if(Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    dummyIndex++;
                }
            }
            auto itermicroPoint = miniTree.microDummyPointers.cbegin();
            Bitvector subTreeB = Bitvector_Utils::getEntry(itermicroPoint, dummyIndex, miniTree.microDummyPointers.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator});
            microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }

    if((std::get<1>(node) > 0 || std::get<2>(node) > 0) && (microNum != miniDummyTree || nodeNum != miniDummyIndex)) {
        auto iterm = leftmostMini.microTreeRightmostLeafPointers.cbegin();
        Bitvector leftmostMicro = Bitvector_Utils::getEntry(iterm,microNum,leftmostMini.microTreeRightmostLeafPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        auto iter = entry.rightmost_leaf.cbegin();
        Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.rightmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        nodeNum = Bitvector_Utils::decodeNumber(entryleftmost, Bitvector_Utils::NumberEncoding::BINARY);

        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY));
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }
    miniNum = Bitvector_Utils::decodeNumber(miniTree.miniTreeRightmostLeafPointer,Bitvector_Utils::NumberEncoding::BINARY);
    leftmostMini = getMiniTree(miniNum);
    auto iterm = leftmostMini.microTreeRightmostLeafPointers.cbegin();
    Bitvector leftmostMicro = Bitvector_Utils::getEntry(iterm,0,leftmostMini.microTreeRightmostLeafPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
    microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini,microNum));
    auto iter = entry.rightmost_leaf.cbegin();
    Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.rightmost_leaf.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator});
    nodeNum = Bitvector_Utils::decodeNumber(entryleftmost,Bitvector_Utils::NumberEncoding::BINARY);
    return {miniNum, microNum, nodeNum};
}

uint32_t HypersuccinctTree::leaf_size(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<2>(node) > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMicroTree(node)) {
            isDummyAncestor = 1;
            auto dummyIter = miniTree.dummys.cbegin();
            uint32_t dummyIndex = 0;
            for(int i=0; i<std::get<1>(node);i++) {
                Bitvector dummy = getMicroDummys(miniTree, 0);
                if(Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    dummyIndex++;
                }
            }
            auto iterDummy = miniTree.microDummyPointers.cbegin();
            Bitvector newMicroBit = Bitvector_Utils::getEntry(iterDummy,dummyIndex,miniTree.microDummyPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
            uint32_t newMicro = Bitvector_Utils::decodeNumber(newMicroBit,Bitvector_Utils::NumberEncoding::BINARY);
            auto iterm = miniTree.microLeaves.cbegin();
            Bitvector microLeafSize = Bitvector_Utils::getEntry(iterm,newMicro,miniTree.microLeaves.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
            res += Bitvector_Utils::decodeNumber(microLeafSize, Bitvector_Utils::NumberEncoding::BINARY);

        }

        if (isDummyAncestorWithinMiniTree(node)) {
            isDummyAncestor = 1;
            uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            MiniTree newMiniTree = getMiniTree(newMini);
            res += Bitvector_Utils::decodeNumber(newMiniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        auto iter = entry.leaves.cbegin();
        Bitvector entryLeafSize = Bitvector_Utils::getEntry(iter, std::get<2>(node), entry.leaves.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator});
        return res + Bitvector_Utils::decodeNumber(entryLeafSize, Bitvector_Utils::NumberEncoding::BINARY);
    }
    if(std::get<1>(node) > 0) {
        if(isDummyAncestorWithinMiniTree({std::get<0>(node),std::get<1>(node),0})) {
            uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            MiniTree newMiniTree = getMiniTree(newMini);
            res += Bitvector_Utils::decodeNumber(newMiniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
        }
        auto iter = miniTree.microLeaves.cbegin();
        Bitvector microLeafSize = Bitvector_Utils::getEntry(iter,std::get<1>(node),miniTree.microLeaves.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        return res + Bitvector_Utils::decodeNumber(microLeafSize, Bitvector_Utils::NumberEncoding::BINARY);
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
}

HstNode HypersuccinctTree::levelAncestor(uint32_t level, HstNode node) {
    if(std::get<2>(node) > 0) {
        //depth via lookputable
        uint32_t depth = 0;
        if(level <= depth) {
            //ancestormatrix oder so
            return {std::get<0>(node),std::get<1>(node),0};
        }
        return levelAncestor(level-depth, {std::get<0>(node),std::get<1>(node),0});
    }
    if(std::get<1>(node) > 0) {
        //TreeSize or microTreeSize???
        uint32_t delta = ceil(sqrt(log2(Bitvector_Utils::decodeNumber(size, Bitvector_Utils::NumberEncoding::BINARY))));
        uint32_t depth = 0; //depth of MicroTree in MiniTree
        if(level > depth) {
            return levelAncestor(level-depth, {std::get<0>(node),0,0});
        }
        //???
    }
    //uint32_t delta =
    //M is miniTree Size?
    uint32_t delta = ceil(sqrt(Bitvector_Utils::decodeNumber(miniSize,Bitvector_Utils::NumberEncoding::BINARY)));
    return {};
}

HstNode findParent(HstNode node) {
    assert(std::get<0>(node) != 0);
    if(std::get<2>(node) != 0) {

    }
    if(std::get<1>(node) != 0) {

    }
    //for(uint32_t i=0, i < )
    return {0,0,0};
}

uint32_t HypersuccinctTree::childRank(HstNode node) {
    //TODO: find Parent
    return 0;
}
