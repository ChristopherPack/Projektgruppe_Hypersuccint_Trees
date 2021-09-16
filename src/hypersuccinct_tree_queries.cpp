//
// Created by Christopher Pack on 20.05.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>

using namespace pht;

bool HypersuccinctTree::isDummyAncestorWithinMiniTree(HstNode node) {
    //assert(node.micro <= Bitvector_Utils::getEntryCount(miniTrees.at(node.mini).microTrees.cbegin(),miniTrees.at(node.mini).microTrees.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {2, 0, Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator()}));
    MiniTree mini = getMiniTree(node.mini);
    if(mini.rootAncestors.empty()) {
        return false;
    }
    HstNode dummy = {node.mini, Bitvector_Utils::decodeNumber(mini.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) , Bitvector_Utils::decodeNumber(mini.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY)};
    assert(dummy.node <= (getMicroTree(mini, dummy.micro).size()/2));
    if(node.node == 0) {
        //explicitly stored Root is Ancestor
        Bitvector rootAnc = mini.rootAncestors;
        if(rootAnc.at(node.micro)) {
            return true;
        }
        return false;
    }
    else if(node.micro == dummy.micro) {
        //lookupTable
        Bitvector index = getMicroTree(mini, node.micro);
        return lookupTableAncestorMatrixComparison(getLookupTableEntry(index), node.node, dummy.node);
    }
    else {
        //explicitly stored - microTreeDummyNode is Ancestor of MiniDummy
        Bitvector dummyAnc = mini.dummyAncestors;
        if(dummyAnc.at(node.micro)) {
            return true;
        }
        return false;
    }
    return false;
}

bool HypersuccinctTree::isDummyAncestorWithinMicroTree(HstNode node) {
    MiniTree mini = getMiniTree(node.mini);
    Bitvector dummy = getMicroDummys(mini,node.micro);
    uint32_t dummyNum = Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY);
    if(dummyNum == 0) {
        return false;
    }
    return lookupTableAncestorMatrixComparison(getLookupTableEntry(getMicroTree(mini, node.micro)),
                                               node.node, dummyNum);
}

HstNode HypersuccinctTree::child(HstNode parent, uint32_t index) {
    MiniTree miniTree = getMiniTree(parent.mini);
    if(degree(parent) < index) {
        return {};
    }
    if(parent.node > 0) {
        if(Bitvector_Utils::decodeNumber(getMiniDummy(parent.mini),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            if(Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == parent.micro && Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == parent.node) {
                return child({Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY),0,0},index);
            }
        }

        uint32_t microDummy = Bitvector_Utils::decodeNumber(getMicroDummys(miniTree, parent.micro),Bitvector_Utils::NumberEncoding::BINARY);
        if(microDummy != 0 && parent.node == microDummy) {
            Bitvector newMicroBit = getMicroDummyPointers(miniTree,parent.micro);
            return child({parent.mini,Bitvector_Utils::decodeNumber(newMicroBit,Bitvector_Utils::NumberEncoding::BINARY),0},index);
        }


        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,parent.micro));
        auto iter = entry.childMatrix.cbegin();
        uint32_t size = sqrt(entry.childMatrix.size());
        Bitvector row = Bitvector_Utils::getEntry(iter, parent.node, entry.childMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 0, size});
        uint32_t countIndex = 0;
        for(uint32_t i=0; i<row.size(); i++) {
            if(row.at(i)) {
                if(index==countIndex) {
                    return {parent.mini,parent.micro,i};
                }
                countIndex++;
            }
        }
        return {};

    }
    if(parent.micro > 0) {
        //TODO: Remove convertMicro
        uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree, parent.micro).first;
        Bitvector fid = miniTree.FIDs.at(fidIndex);
        Bitvector tv = miniTree.typeVectors.at(fidIndex);
        std::pair< std::vector<uint32_t >,std::vector<uint32_t > > trees = getTreesForMicroFID(miniTree ,parent.micro);
        uint32_t countIndex = 0;
        auto tvIter = tv.cbegin();
        uint32_t childNum = 1;
        uint32_t tree;
        for(bool b: fid) {
            if(b) {
                childNum = 1;
                bool tvBit = *tvIter;
                tvIter++;
                if(tvBit) {
                    tree = trees.first.front();
                    trees.first.erase(trees.first.cbegin());
                    if (index == countIndex) {
                        return {parent.mini,tree,childNum};
                    }
                } else
                {
                    tree = trees.second.front();
                    trees.second.erase(trees.second.cbegin());
                    if (index == countIndex) {
                        return {parent.mini,tree,0};
                    }
                }
            }
            else {
                childNum++;
                if (index == countIndex) {
                    return {parent.mini,tree,childNum};
                }
            }
            countIndex++;
        }
    }
    std::pair<uint32_t ,uint32_t > fidIndices = convertTreeToFIDIndex(parent.mini);
    Bitvector fid = miniFIDs.at(fidIndices.first);
    Bitvector tv = miniTypeVectors.at(fidIndices.first);
    std::pair<std::vector<uint32_t>, std::vector<uint32_t> > trees = getTreesForFID(fidIndices.first);
    uint32_t countIndex = 0;
    auto tvIter = tv.cbegin();
    uint32_t childNum = 1;
    uint32_t tree;
    for(bool b: fid) {
        if(b) {
            childNum = 1;
            bool tvBit = *tvIter;
            tvIter++;
            if(tvBit) {
                tree = trees.first.front();
                trees.first.erase(trees.first.cbegin());
                if (index == countIndex) {
                    return {tree,childNum,0};
                }
            } else
            {
                tree = trees.second.front();
                trees.second.erase(trees.second.cbegin());
                if (index == countIndex) {
                    return {tree,0,0};
                }
            }
        }
        else {
            childNum++;
            if (index == countIndex) {
                return {tree,childNum,0};
            }
        }
        countIndex++;
    }
    return {};
}

uint32_t HypersuccinctTree::childRank(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.mini == 0 && node.micro == 0 && node.node == 0) {
        return 0;
    }
    HstNode parent = getParent(node);
    MiniTree miniTreeParent = getMiniTree(parent.mini);
    if(Bitvector_Utils::decodeNumber(getMiniDummy(parent.mini),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == parent.micro && Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == parent.node) {
            return childRank(parent);
        }
    }
    //TODO: should check if node.node is microDummy
    if(Bitvector_Utils::decodeNumber(getMicroDummys(miniTreeParent, parent.micro),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        Bitvector newMicroBit = getMicroDummyPointers(miniTree,parent.micro);
    }
    if(parent.micro == 0 && parent.node== 0) {
        std::pair<uint32_t ,uint32_t > fidIndices = {Bitvector_Utils::decodeNumber(miniTree.miniTopFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1,Bitvector_Utils::decodeNumber(miniTree.miniLowFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1};
        std::pair<std::vector<uint32_t >,std::vector<uint32_t> > parentTrees = getTreesForFID(parent.mini);
        if(!ListUtils::containsAny(parentTrees.second,{parent.mini})) {
            Bitvector fid = miniFIDs.at(fidIndices.first);
            Bitvector tv = miniTypeVectors.at(fidIndices.first);
            std::pair<std::vector<uint32_t>, std::vector<uint32_t> > trees = getTreesForFID(parent.mini);
            for (bool b: tv) {
                if (b) {
                    uint32_t tree = trees.first.front();
                    trees.first.erase(trees.first.cbegin());
                    if (tree == node.mini) {
                        break;
                    } else {
                        res++;
                    }
                } else {
                    uint32_t tree = trees.second.front();
                    trees.second.erase(trees.second.cbegin());
                    if (tree == node.mini) {
                        break;
                    } else {
                        res++;
                    }
                }
            }
        }
    }
    if(parent.node == 0) {
        //TODO: Remove convertMicro
        uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree, parent.micro).first;
        Bitvector fid = miniTree.FIDs.at(fidIndex);
        Bitvector tv = miniTree.typeVectors.at(fidIndex);
        std::pair< std::vector<uint32_t >,std::vector<uint32_t > > trees = getTreesForMicroFID(miniTree ,parent.micro);
        for(bool b: tv) {
            if(b) {
                uint32_t tree = trees.first.front();
                trees.first.erase(trees.first.cbegin());
                if(tree == node.micro) {
                    break;
                }
                else {
                    res++;
                }
            }
            else {
                uint32_t tree = trees.second.front();
                trees.second.erase(trees.second.cbegin());
                if(tree == node.micro) {
                    break;
                }
                else {
                    res++;
                }
            }
        }
    }
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTreeParent,parent.micro));
    auto iter2 = entry.childMatrix.cbegin();
    uint32_t size = sqrt(entry.childMatrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter2, parent.node, entry.childMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 0, size});
    for(int i = 0; i < segment.size(); i++) {
        if(segment.at(i)) {
            if(i == node.node) {
                return res;
            }
            else {
                res++;
            }
        }
    }
    return res;
}

HstNode HypersuccinctTree::getParent(HstNode node) {
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        //TODO: This has to be changed
        auto iter = entry.childMatrix.cbegin();
        uint32_t size = sqrt(entry.childMatrix.size());
        Bitvector column = Bitvector_Utils::getEntry(iter, node.node, entry.childMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC_MATRIX_COLUMN, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 0, size});
        uint32_t parent;
        for(int i = 0; i < column.size(); i++) {
            if(column.at(i)) {
                parent = i;
            }
        }
        //TODO: END OF BLOCK
        return {node.mini,node.micro,parent};
    }
    if(node.micro > 0) {
        //TODO: Remove convertMicro
        uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree,node.micro).second;
        if(fidIndex == -1) {
            if(node.micro == 0) {
                return {static_cast<uint32_t>(-1),static_cast<uint32_t>(-1),static_cast<uint32_t>(-1)};
            }
            for(int i=0; i< getMicroTreeCount(miniTree);i++) {
                Bitvector microDummy = getMicroDummys(miniTree,i);
                uint32_t microDummyPointerIndex = 0;
                if(Bitvector_Utils::decodeNumber(microDummy, Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    Bitvector newMicroBit = getMicroDummyPointers(miniTree,i);
                    std::pair< std::vector<uint32_t> , std::vector<uint32_t >> trees = getTreesForMicroFID(miniTree,node.micro);
                    if(ListUtils::containsAny(trees.first,{Bitvector_Utils::decodeNumber(newMicroBit,Bitvector_Utils::NumberEncoding::BINARY)})) {
                        return {node.mini,static_cast<uint32_t>(i), Bitvector_Utils::decodeNumber(microDummy,Bitvector_Utils::NumberEncoding::BINARY)};
                    }
                    microDummyPointerIndex++;
                }
            }
        }
        std::pair< std::vector<uint32_t >,std::vector<uint32_t > > trees = getTreesForMicroFID(miniTree, fidIndex);
        return {node.mini,trees.first.front(),0};
    }
    uint32_t fidIndex = Bitvector_Utils::decodeNumber(miniTree.miniLowFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1;

    if(fidIndex == -1) {
        if(node.mini == 0) {
            return {static_cast<uint32_t>(-1),static_cast<uint32_t>(-1),static_cast<uint32_t>(-1)};
        }
        for(int i=0; i<miniTrees.size();i++) {
            Bitvector miniDummy = getMiniDummy(i);
            if(Bitvector_Utils::decodeNumber(miniDummy, Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                MiniTree mini = getMiniTree(i);
                std::pair< std::vector<uint32_t> , std::vector<uint32_t >> trees = getTreesForFID(node.mini);
                if(ListUtils::containsAny(trees.first,{Bitvector_Utils::decodeNumber(mini.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY)})) {
                    return {static_cast<uint32_t>(i),Bitvector_Utils::decodeNumber(mini.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY), Bitvector_Utils::decodeNumber(mini.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY)};
                }
            }
        }
    }

    uint32_t firstTree = Bitvector_Utils::decodeNumber(FIDTopTree.at(fidIndex),Bitvector_Utils::NumberEncoding::BINARY);
    return {firstTree,0,0};
}

uint32_t HypersuccinctTree::degree(HstNode node) {
    if(node.node > 0) {
        MiniTree miniTree = getMiniTree(node.mini);
        if(Bitvector_Utils::decodeNumber(getMicroDummys(miniTree,node.micro),Bitvector_Utils::NumberEncoding::BINARY) == node.node) {
            Bitvector subTreeB = getMicroDummyPointers(miniTree,node.micro);
            uint32_t microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
            return degree({node.mini,microNum,0});
        }
        if(Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == node.micro && Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == node.node ) {
            uint32_t miniPoint = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            return degree({miniPoint,0,0});
        }
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        //auto iter = entry.degree.cbegin();
        //Bitvector degreeB = Bitvector_Utils::getEntry(iter,node.node,entry.degree.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector degreeB = entry.degree.at(node.node);
        return Bitvector_Utils::decodeNumber(degreeB,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(node.micro > 0) {
        Bitvector fid = getFIDforMicroTree(node.mini, node.micro);
        return fid.size();
    }
    uint32_t fidID = Bitvector_Utils::decodeNumber(getMiniTree(node.mini).miniTopFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1;
    Bitvector fid = miniFIDs.at(fidID);
    return fid.size();
}

uint32_t HypersuccinctTree::subtreeSize(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMicroTree(node)) {
            isDummyAncestor = 1;
            Bitvector subTreeB = getMicroDummyPointers(miniTree,node.micro);
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
            Bitvector subtreeSize = miniTree.microSubTrees.at(dummyPointer);
            res += Bitvector_Utils::decodeNumber(subtreeSize, Bitvector_Utils::NumberEncoding::BINARY);
        }
        if(isDummyAncestorWithinMiniTree(node)) {
            isDummyAncestor = 1;
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            res += Bitvector_Utils::decodeNumber(getMiniTree(dummyPointer).subTree, Bitvector_Utils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        //auto iter = entry.subTrees.cbegin();
        //Bitvector degreeB = Bitvector_Utils::getEntry(iter,node.node,entry.subTrees.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector degreeB = entry.subTrees.at(node.node);
        return res + Bitvector_Utils::decodeNumber(degreeB,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(node.micro > 0) {
        if(isDummyAncestorWithinMiniTree(node)) {
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            res += Bitvector_Utils::decodeNumber(getMiniTree(dummyPointer).subTree, Bitvector_Utils::NumberEncoding::BINARY);
        }
        Bitvector subTreeB = miniTree.microSubTrees.at(node.micro);
        return res + Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.subTree,Bitvector_Utils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::depth(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        //auto iter = entry.nodeDepths.cbegin();
        //Bitvector entryDepth = Bitvector_Utils::getEntry(iter,node.node,entry.nodeDepths.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector entryDepth = entry.nodeDepths.at(node.node);
        res += Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(node.micro > 0) {
        Bitvector microDepth = miniTree.rootDepths.at(node.micro);
        res += Bitvector_Utils::decodeNumber(microDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniDepth, Bitvector_Utils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::height(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        if(isDummyAncestorWithinMicroTree(node) || isDummyAncestorWithinMiniTree(node)) {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
            //auto iter = entry.nodeDepths.cbegin();
            //Bitvector entryDepth = Bitvector_Utils::getEntry(iter,node.node,entry.nodeDepths.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
            Bitvector entryDepth = entry.nodeDepths.at(node.node);
            res -= (Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1);
        }
        else {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
            //auto iter = entry.nodeHeights.cbegin();
            //Bitvector entryDepth = Bitvector_Utils::getEntry(iter,node.node,entry.nodeHeights.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
            Bitvector entryDepth = entry.nodeHeights.at(node.node);
            return res + Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
    }
    if(node.micro > 0) {
        if(isDummyAncestorWithinMiniTree({node.mini,node.micro,0})) {
            Bitvector microDepth = miniTree.rootDepths.at(node.micro);
            res -= (Bitvector_Utils::decodeNumber(microDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1);
        }
        else {
            Bitvector microDepth = miniTree.rootHeights.at(node.micro);
            return res + Bitvector_Utils::decodeNumber(microDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniHeight, Bitvector_Utils::NumberEncoding::BINARY);
}

HstNode HypersuccinctTree::leftmostLeaf(HstNode node) {
    uint32_t miniNum = node.mini;
    uint32_t microNum = node.micro;
    uint32_t nodeNum = node.node;
    MiniTree miniTree = getMiniTree(node.mini);
    MiniTree leftmostMini = miniTree;
    uint32_t miniDummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t miniDummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);

    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        //auto iter = entry.leftmost_leaf.cbegin();
        //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, nodeNum, entry.leftmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector entryleftmost = entry.leftmost_leaf.at(nodeNum);
        nodeNum = Bitvector_Utils::decodeNumber(entryleftmost, Bitvector_Utils::NumberEncoding::BINARY);
        Bitvector microDummy = getMicroDummys(miniTree, microNum);
        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY));
        }
        else if (nodeNum == Bitvector_Utils::decodeNumber(microDummy, Bitvector_Utils::NumberEncoding::BINARY)) {
            Bitvector subTreeB = getMicroDummyPointers(miniTree,node.micro);
            microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }

    if((node.micro > 0 || node.node > 0)  && (microNum != miniDummyTree || nodeNum != miniDummyIndex)) {
        Bitvector leftmostMicro = leftmostMini.microTreeLeftmostLeafPointers.at(microNum);
        microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        //auto iter = entry.leftmost_leaf.cbegin();
        //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.leftmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector entryleftmost = entry.leftmost_leaf.at(0);
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
    Bitvector leftmostMicro = leftmostMini.microTreeLeftmostLeafPointers.at(0);
    microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini,microNum));
    //auto iter = entry.leftmost_leaf.cbegin();
    //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.leftmost_leaf.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator()});
    Bitvector entryleftmost = entry.leftmost_leaf.at(0);
    nodeNum = Bitvector_Utils::decodeNumber(entryleftmost,Bitvector_Utils::NumberEncoding::BINARY);
    return {miniNum, microNum, nodeNum};
}

HstNode HypersuccinctTree::rightmostLeaf(HstNode node) {
    uint32_t miniNum = node.mini;
    uint32_t microNum = node.micro;
    uint32_t nodeNum = node.node;
    MiniTree miniTree = getMiniTree(node.mini);
    MiniTree leftmostMini = miniTree;
    uint32_t miniDummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t miniDummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);

    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        //auto iter = entry.rightmost_leaf.cbegin();
        //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, nodeNum, entry.rightmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector entryleftmost = entry.rightmost_leaf.at(nodeNum);
        nodeNum = Bitvector_Utils::decodeNumber(entryleftmost, Bitvector_Utils::NumberEncoding::BINARY);
        Bitvector microDummy = getMicroDummys(miniTree, microNum);
        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY));
        }
        else if (nodeNum == Bitvector_Utils::decodeNumber(microDummy, Bitvector_Utils::NumberEncoding::BINARY)) {
            Bitvector subTreeB = getMicroDummyPointers(miniTree,node.micro);
            microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }

    if((node.micro > 0 || node.node > 0) && (microNum != miniDummyTree || nodeNum != miniDummyIndex)) {
        Bitvector leftmostMicro = leftmostMini.microTreeRightmostLeafPointers.at(microNum);
        microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        //auto iter = entry.rightmost_leaf.cbegin();
        //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.rightmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector entryleftmost = entry.rightmost_leaf.at(0);
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
    Bitvector leftmostMicro = leftmostMini.microTreeRightmostLeafPointers.at(0);
    microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini,microNum));
    //auto iter = entry.rightmost_leaf.cbegin();
    //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.rightmost_leaf.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator()});
    Bitvector entryleftmost = entry.rightmost_leaf.at(0);
    nodeNum = Bitvector_Utils::decodeNumber(entryleftmost,Bitvector_Utils::NumberEncoding::BINARY);
    return {miniNum, microNum, nodeNum};
}

uint32_t HypersuccinctTree::leafSize(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMicroTree(node)) {
            isDummyAncestor = 1;
            Bitvector newMicroBit = getMicroDummyPointers(miniTree,node.micro);
            uint32_t newMicro = Bitvector_Utils::decodeNumber(newMicroBit,Bitvector_Utils::NumberEncoding::BINARY);
            Bitvector microLeafSize = miniTree.microLeaves.at(newMicro);
            res += Bitvector_Utils::decodeNumber(microLeafSize, Bitvector_Utils::NumberEncoding::BINARY);

        }

        if (isDummyAncestorWithinMiniTree(node)) {
            isDummyAncestor = 1;
            uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            MiniTree newMiniTree = getMiniTree(newMini);
            res += Bitvector_Utils::decodeNumber(newMiniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        //auto iter = entry.leaves.cbegin();
        //Bitvector entryLeafSize = Bitvector_Utils::getEntry(iter, node.node, entry.leaves.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator()});
        Bitvector entryLeafSize = entry.leaves.at(node.node);
        return res + Bitvector_Utils::decodeNumber(entryLeafSize, Bitvector_Utils::NumberEncoding::BINARY);
    }
    if(node.micro > 0) {
        if(isDummyAncestorWithinMiniTree({node.mini,node.micro,0})) {
            uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            MiniTree newMiniTree = getMiniTree(newMini);
            res += Bitvector_Utils::decodeNumber(newMiniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
        }
        Bitvector microLeafSize = miniTree.microLeaves.at(node.micro);
        return res + Bitvector_Utils::decodeNumber(microLeafSize, Bitvector_Utils::NumberEncoding::BINARY);
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::leafRank(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);

    if(node.node > 0) {
        uint32_t dummyMarker = 0;
        if(isDummyAncestorWithinMicroTree({node.mini,node.micro,0})) {
            Bitvector dummy = getMicroDummys(miniTree,node.micro);
            uint32_t dummyNum = Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY);
            if(dummyNum == node.node) {
                dummyMarker = 1;
            }
            if(dummyNum < node.node) {
                Bitvector newMicroBit = getMicroDummyPointers(miniTree,node.micro);
                uint32_t newMicro = Bitvector_Utils::decodeNumber(newMicroBit, Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector leaf_rank_node = miniTree.microRootLeafRanks.at(newMicro);
                res += Bitvector_Utils::decodeNumber(leaf_rank_node, Bitvector_Utils::NumberEncoding::BINARY) - 1;
            }
        }
        if(isDummyAncestorWithinMiniTree({node.mini,0,0})) {
            Bitvector dummy = getMiniDummy(node.mini);
            uint32_t dummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree, Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t dummyNum = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);
            if(dummyTree == node.micro && dummyNum == node.node) {
                dummyMarker = 1;
            }
            if(dummyTree <= node.micro && dummyNum < node.node) {
                uint32_t newMiniNum = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
                MiniTree newMini = getMiniTree(newMiniNum);
                res += Bitvector_Utils::decodeNumber(newMini.miniRootLeafRank,Bitvector_Utils::NumberEncoding::BINARY);
            }
        }

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        //auto iter = entry.leafRank.cbegin();
        //Bitvector leaf_rank_node = Bitvector_Utils::getEntry(iter,node.node,entry.leafRank.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector leaf_rank_node = entry.leafRank.at(node.node);
        res += Bitvector_Utils::decodeNumber(leaf_rank_node, Bitvector_Utils::NumberEncoding::BINARY) - 1 - dummyMarker;
    }

    if(node.micro > 0) {
        if(isDummyAncestorWithinMiniTree({node.mini,0,0})) {
            uint32_t dummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t dummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);
            if(dummyTree < node.micro || (dummyTree == node.micro&&dummyIndex < node.node)) {
                uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
                MiniTree newMiniTree = getMiniTree(newMini);
                res += Bitvector_Utils::decodeNumber(newMiniTree.miniDummyLeafRank, Bitvector_Utils::NumberEncoding::BINARY);
            }
        }



        if (node.node!=0 && (node.micro + node.node > 1)) {
            //TODO: Need same thing als below, just for microTrees
            res += Bitvector_Utils::decodeNumber(miniTree.microExtendedLeafRanks.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
            /*uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree, node.micro).first;
            uint32_t fidIndex = Bitvector_Utils::decodeNumber(miniTree.microTopFIDIndices.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY);
            Bitvector fid = miniTree.FIDs.at(fidIndex);
            uint32_t fidSize = fid.size();
            Bitvector tv = miniTree.typeVectors.at(fidIndex);
            uint32_t firstTop = Bitvector_Utils::decodeNumber(miniTree.microFIDTopTrees.at(fidIndex),Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t firstLow = Bitvector_Utils::decodeNumber(miniTree.microFIDLowTrees.at(fidIndex),Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t amountTrees = miniTree.FIDsSupport.at(fidIndex).Rank(fidIndex);
            uint32_t searchTree = node.micro;

            for (bool b: tv) {
                if (b) {
                    uint32_t tree = trees.first.front();
                    trees.first.erase(trees.first.cbegin());
                    if (tree == node.micro) {
                        break;
                    } else {
                        res += leafSize({node.mini, tree, 0});
                    }
                } else {
                    uint32_t tree = trees.second.front();
                    trees.second.erase(trees.second.cbegin());
                    if (tree == node.micro) {
                        break;
                    } else {
                        res += leafSize({node.mini, tree, 0});
                    }
                }
            }*/
        } else {
            res += (Bitvector_Utils::decodeNumber(miniTree.microRootLeafRanks.at(node.micro), Bitvector_Utils::NumberEncoding::BINARY) - 1);
        }

    }



    //if(childRank(node) > 0) {
    if ((node.micro != 0 || node.node != 0)) {
        res += Bitvector_Utils::decodeNumber(miniTree.microRootLeafRanks.at(0),Bitvector_Utils::NumberEncoding::BINARY) - 1;
    } else {
        res += Bitvector_Utils::decodeNumber(miniTree.miniRootLeafRank, Bitvector_Utils::NumberEncoding::BINARY);
    }
    //}


    return res;
}



HstNode HypersuccinctTree::levelAncestor(HstNode node, uint32_t level) {
    //TODO:
    if(node.node > 0) {
        //depth via lookputable
        uint32_t depth = 0;
        if(level <= depth) {
            //ancestormatrix oder so
            return {node.mini,node.micro,0};
        }
        return levelAncestor({node.mini, node.micro, 0}, level - depth);
    }
    if(node.micro > 0) {
        //TreeSize or microTreeSize???
        uint32_t delta = ceil(sqrt(log2(Bitvector_Utils::decodeNumber(size, Bitvector_Utils::NumberEncoding::BINARY))));
        uint32_t depth = 0; //depth of MicroTree in MiniTree
        if(level > depth) {
            return levelAncestor({node.mini, 0, 0}, level - depth);
        }
        //???
    }
    //uint32_t delta =
    //M is miniTree Size?
    uint32_t delta = ceil(sqrt(Bitvector_Utils::decodeNumber(miniSize,Bitvector_Utils::NumberEncoding::BINARY)));
    return {};
}
