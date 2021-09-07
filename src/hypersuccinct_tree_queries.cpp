//
// Created by Christopher Pack on 20.05.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>

using namespace pht;

bool HypersuccinctTree::isDummyAncestorWithinMiniTree(HstNode node) {
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
        return lookupTableAncestorMatrixComparison(getLookupTableEntry(index), std::get<2>(node), std::get<2>(dummy));
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
    return lookupTableAncestorMatrixComparison(getLookupTableEntry(getMicroTree(mini, std::get<1>(node))),
                                               std::get<2>(node), dummyNum);
}

HstNode HypersuccinctTree::child(HstNode parent, uint32_t index) {
    MiniTree miniTree = getMiniTree(std::get<0>(parent));
    if(degree(parent) < index) {
        return {};
    }
    if(std::get<2>(parent) > 0) {
        if(Bitvector_Utils::decodeNumber(getMiniDummy(std::get<0>(parent)),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            if(Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == std::get<1>(parent) && Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == std::get<2>(parent)) {
                return child({Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY),0,0},index);
            }
        }

        uint32_t microDummy = Bitvector_Utils::decodeNumber(getMicroDummys(miniTree, std::get<1>(parent)),Bitvector_Utils::NumberEncoding::BINARY);
        if(microDummy != 0 && std::get<2>(parent) == microDummy) {
            Bitvector newMicroBit = getMicroDummyPointers(miniTree,std::get<1>(parent));
            return child({std::get<0>(parent),Bitvector_Utils::decodeNumber(newMicroBit,Bitvector_Utils::NumberEncoding::BINARY),0},index);
        }


        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(parent)));
        auto iter = entry.childMatrix.cbegin();
        uint32_t size = sqrt(entry.childMatrix.size());
        Bitvector row = Bitvector_Utils::getEntry(iter, std::get<2>(parent), entry.childMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 0, size});
        uint32_t countIndex = 0;
        for(uint32_t i=0; i<row.size(); i++) {
            if(row.at(i)) {
                if(index==countIndex) {
                    return {std::get<0>(parent),std::get<1>(parent),i};
                }
                countIndex++;
            }
        }
        return {};

    }
    if(std::get<1>(parent) > 0) {
        //TODO: Remove convertMicro
        uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree, std::get<1>(parent)).first;
        Bitvector fid = miniTree.FIDs.at(fidIndex);
        Bitvector tv = miniTree.typeVectors.at(fidIndex);
        std::pair< std::vector<uint32_t >,std::vector<uint32_t > > trees = getTreesForMicroFID(miniTree ,std::get<1>(parent));
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
                        return {std::get<0>(parent),tree,childNum};
                    }
                } else
                {
                    tree = trees.second.front();
                    trees.second.erase(trees.second.cbegin());
                    if (index == countIndex) {
                        return {std::get<0>(parent),tree,0};
                    }
                }
            }
            else {
                childNum++;
                if (index == countIndex) {
                    return {std::get<0>(parent),tree,childNum};
                }
            }
            countIndex++;
        }
    }
    std::pair<uint32_t ,uint32_t > fidIndices = convertTreeToFIDIndex(std::get<0>(parent));
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
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<0>(node) == 0 && std::get<1>(node) == 0 && std::get<2>(node) == 0) {
        return 0;
    }
    HstNode parent = getParent(node);
    MiniTree miniTreeParent = getMiniTree(std::get<0>(parent));
    if(Bitvector_Utils::decodeNumber(getMiniDummy(std::get<0>(parent)),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == std::get<1>(parent) && Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == std::get<2>(parent)) {
            return childRank(parent);
        }
    }
    if(Bitvector_Utils::decodeNumber(getMicroDummys(miniTreeParent, std::get<1>(parent)),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        Bitvector newMicroBit = getMicroDummyPointers(miniTree,std::get<1>(parent));
    }
    if(std::get<1>(parent) == 0 && std::get<2>(parent)== 0) {
        std::pair<uint32_t ,uint32_t > fidIndices = {Bitvector_Utils::decodeNumber(miniTree.miniTopFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1,Bitvector_Utils::decodeNumber(miniTree.miniLowFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1};
        std::pair<std::vector<uint32_t >,std::vector<uint32_t> > parentTrees = getTreesForFID(std::get<0>(parent));
        if(!ListUtils::containsAny(parentTrees.second,{std::get<0>(parent)})) {
            Bitvector fid = miniFIDs.at(fidIndices.first);
            Bitvector tv = miniTypeVectors.at(fidIndices.first);
            std::pair<std::vector<uint32_t>, std::vector<uint32_t> > trees = getTreesForFID(std::get<0>(parent));
            for (bool b: tv) {
                if (b) {
                    uint32_t tree = trees.first.front();
                    trees.first.erase(trees.first.cbegin());
                    if (tree == std::get<0>(node)) {
                        break;
                    } else {
                        res++;
                    }
                } else {
                    uint32_t tree = trees.second.front();
                    trees.second.erase(trees.second.cbegin());
                    if (tree == std::get<0>(node)) {
                        break;
                    } else {
                        res++;
                    }
                }
            }
        }
    }
    if(std::get<2>(parent) == 0) {
        //TODO: Remove convertMicro
        uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree, std::get<1>(parent)).first;
        Bitvector fid = miniTree.FIDs.at(fidIndex);
        Bitvector tv = miniTree.typeVectors.at(fidIndex);
        std::pair< std::vector<uint32_t >,std::vector<uint32_t > > trees = getTreesForMicroFID(miniTree ,std::get<1>(parent));
        for(bool b: tv) {
            if(b) {
                uint32_t tree = trees.first.front();
                trees.first.erase(trees.first.cbegin());
                if(tree == std::get<1>(node)) {
                    break;
                }
                else {
                    res++;
                }
            }
            else {
                uint32_t tree = trees.second.front();
                trees.second.erase(trees.second.cbegin());
                if(tree == std::get<1>(node)) {
                    break;
                }
                else {
                    res++;
                }
            }
        }
    }
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTreeParent,std::get<1>(parent)));
    auto iter2 = entry.childMatrix.cbegin();
    uint32_t size = sqrt(entry.childMatrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter2, std::get<2>(parent), entry.childMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 0, size});
    for(int i = 0; i < segment.size(); i++) {
        if(segment.at(i)) {
            if(i == std::get<2>(node)) {
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
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<2>(node) > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        //TODO: This Block could become function: LookupTableParentMatrixComparison
        auto iter = entry.childMatrix.cbegin();
        uint32_t size = sqrt(entry.childMatrix.size());
        Bitvector column = Bitvector_Utils::getEntry(iter, std::get<2>(node), entry.childMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC_MATRIX_COLUMN, {Bitvector_Utils::nullIterator(), Bitvector_Utils::nullIterator(), 0, size});
        uint32_t parent;
        for(int i = 0; i < column.size(); i++) {
            if(column.at(i)) {
                parent = i;
            }
        }
        //TODO: END OF BLOCK
        return {std::get<0>(node),std::get<1>(node),parent};
    }
    if(std::get<1>(node) > 0) {
        //TODO: Remove convertMicro
        uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree,std::get<1>(node)).second;
        if(fidIndex == -1) {
            if(std::get<1>(node) == 0) {
                return {-1,-1,-1};
            }
            for(int i=0; i< getMicroTreeCount(miniTree);i++) {
                Bitvector microDummy = getMicroDummys(miniTree,i);
                uint32_t microDummyPointerIndex = 0;
                if(Bitvector_Utils::decodeNumber(microDummy, Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    Bitvector newMicroBit = getMicroDummyPointers(miniTree,i);
                    std::pair< std::vector<uint32_t> , std::vector<uint32_t >> trees = getTreesForMicroFID(miniTree,std::get<1>(node));
                    if(ListUtils::containsAny(trees.first,{Bitvector_Utils::decodeNumber(newMicroBit,Bitvector_Utils::NumberEncoding::BINARY)})) {
                        return {std::get<0>(node),i, Bitvector_Utils::decodeNumber(microDummy,Bitvector_Utils::NumberEncoding::BINARY)};
                    }
                    microDummyPointerIndex++;
                }
            }
        }
        std::pair< std::vector<uint32_t >,std::vector<uint32_t > > trees = getTreesForMicroFID(miniTree, fidIndex);
        return {std::get<0>(node),trees.first.front(),0};
    }
    uint32_t fidIndex = Bitvector_Utils::decodeNumber(miniTree.miniLowFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1;
    if(fidIndex == -1) {
        if(std::get<0>(node) == 0) {
            return {-1,-1,-1};
        }
        for(int i=0; i<miniTrees.size();i++) {
            Bitvector miniDummy = getMiniDummy(i);
            if(Bitvector_Utils::decodeNumber(miniDummy, Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                MiniTree mini = getMiniTree(i);
                std::pair< std::vector<uint32_t> , std::vector<uint32_t >> trees = getTreesForFID(std::get<0>(node));
                if(ListUtils::containsAny(trees.first,{Bitvector_Utils::decodeNumber(mini.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY)})) {
                    return {i,Bitvector_Utils::decodeNumber(mini.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY), Bitvector_Utils::decodeNumber(mini.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY)};
                }
            }
        }
    }
    std::pair< std::vector<uint32_t >,std::vector<uint32_t > > trees = getTreesForFID(fidIndex);
    return {trees.first.front(),0,0};
}

uint32_t HypersuccinctTree::degree(HstNode node) {
    if(std::get<2>(node) > 0) {
        MiniTree miniTree = getMiniTree(std::get<0>(node));
        if(Bitvector_Utils::decodeNumber(getMicroDummys(miniTree,std::get<1>(node)),Bitvector_Utils::NumberEncoding::BINARY) == std::get<2>(node)) {
            Bitvector subTreeB = getMicroDummyPointers(miniTree,std::get<1>(node));
            uint32_t microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
            return degree({std::get<0>(node),microNum,0});
        }
        if(Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == std::get<1>(node) && Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == std::get<2>(node) ) {
            uint32_t miniPoint = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            return degree({miniPoint,0,0});
        }
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        //auto iter = entry.degree.cbegin();
        //Bitvector degreeB = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.degree.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector degreeB = entry.degree.at(std::get<2>(node));
        return Bitvector_Utils::decodeNumber(degreeB,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(std::get<1>(node) > 0) {
        Bitvector fid = getFIDforMicroTree(std::get<0>(node), std::get<1>(node));
        return fid.size();
    }
    uint32_t fidID = Bitvector_Utils::decodeNumber(getMiniTree(std::get<0>(node)).miniTopFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1;
    Bitvector fid = miniFIDs.at(fidID);
    return fid.size();
}

uint32_t HypersuccinctTree::subtreeSize(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<2>(node) > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMicroTree(node)) {
            isDummyAncestor = 1;
            Bitvector subTreeB = getMicroDummyPointers(miniTree,std::get<1>(node));
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

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        //auto iter = entry.subTrees.cbegin();
        //Bitvector degreeB = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.subTrees.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector degreeB = entry.subTrees.at(std::get<2>(node));
        return res + Bitvector_Utils::decodeNumber(degreeB,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(std::get<1>(node) > 0) {
        if(isDummyAncestorWithinMiniTree(node)) {
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            res += Bitvector_Utils::decodeNumber(getMiniTree(dummyPointer).subTree, Bitvector_Utils::NumberEncoding::BINARY);
        }
        Bitvector subTreeB = miniTree.microSubTrees.at(std::get<1>(node));
        return res + Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.subTree,Bitvector_Utils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::depth(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<2>(node) > 0) {

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        //auto iter = entry.nodeDepths.cbegin();
        //Bitvector entryDepth = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.nodeDepths.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector entryDepth = entry.nodeDepths.at(std::get<2>(node));
        res += Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(std::get<1>(node) > 0) {
        Bitvector microDepth = miniTree.rootDepths.at(std::get<1>(node));
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
            //auto iter = entry.nodeDepths.cbegin();
            //Bitvector entryDepth = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.nodeDepths.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
            Bitvector entryDepth = entry.nodeDepths.at(std::get<2>(node));
            res -= (Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1);
        }
        else {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
            //auto iter = entry.nodeHeights.cbegin();
            //Bitvector entryDepth = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.nodeHeights.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
            Bitvector entryDepth = entry.nodeDepths.at(std::get<2>(node));
            return res + Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
    }
    if(std::get<1>(node) > 0) {
        if(isDummyAncestorWithinMiniTree({std::get<0>(node),std::get<1>(node),0})) {
            Bitvector microDepth = miniTree.rootDepths.at(std::get<1>(node));
            res -= (Bitvector_Utils::decodeNumber(microDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1);
        }
        else {
            Bitvector microDepth = miniTree.rootDepths.at(std::get<1>(node));
            return res + Bitvector_Utils::decodeNumber(microDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniHeight, Bitvector_Utils::NumberEncoding::BINARY);
}

HstNode HypersuccinctTree::leftmostLeaf(HstNode node) {
    uint32_t miniNum = std::get<0>(node);
    uint32_t microNum = std::get<1>(node);
    uint32_t nodeNum = std::get<2>(node);
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    MiniTree leftmostMini = miniTree;
    uint32_t miniDummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t miniDummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);

    if(std::get<2>(node) > 0) {
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
            Bitvector subTreeB = getMicroDummyPointers(miniTree,std::get<1>(node));
            microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }

    if((std::get<1>(node) > 0 || std::get<2>(node) > 0)  && (microNum != miniDummyTree || nodeNum != miniDummyIndex)) {
        Bitvector leftmostMicro = leftmostMini.microTreeLeftmostLeafPointers.at(microNum);
        microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        //auto iter = entry.leftmost_leaf.cbegin();
        //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.leftmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector entryleftmost = entry.leftmost_leaf.at(nodeNum);
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
    Bitvector entryleftmost = entry.leftmost_leaf.at(nodeNum);
    nodeNum = Bitvector_Utils::decodeNumber(entryleftmost,Bitvector_Utils::NumberEncoding::BINARY);
    return {miniNum, microNum, nodeNum};
}

HstNode HypersuccinctTree::rightmostLeaf(HstNode node) {
    uint32_t miniNum = std::get<0>(node);
    uint32_t microNum = std::get<1>(node);
    uint32_t nodeNum = std::get<2>(node);
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    MiniTree leftmostMini = miniTree;
    uint32_t miniDummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
    uint32_t miniDummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);

    if(std::get<2>(node) > 0) {
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
            Bitvector subTreeB = getMicroDummyPointers(miniTree,std::get<1>(node));
            microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }

    if((std::get<1>(node) > 0 || std::get<2>(node) > 0) && (microNum != miniDummyTree || nodeNum != miniDummyIndex)) {
        Bitvector leftmostMicro = leftmostMini.microTreeRightmostLeafPointers.at(microNum);
        microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        //auto iter = entry.rightmost_leaf.cbegin();
        //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.rightmost_leaf.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector entryleftmost = entry.rightmost_leaf.at(nodeNum);
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
    Bitvector leftmostMicro = leftmostMini.microTreeLeftmostLeafPointers.at(0);
    microNum = Bitvector_Utils::decodeNumber(leftmostMicro,Bitvector_Utils::NumberEncoding::BINARY);
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini,microNum));
    //auto iter = entry.rightmost_leaf.cbegin();
    //Bitvector entryleftmost = Bitvector_Utils::getEntry(iter, 0, entry.rightmost_leaf.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator()});
    Bitvector entryleftmost = entry.rightmost_leaf.at(nodeNum);
    nodeNum = Bitvector_Utils::decodeNumber(entryleftmost,Bitvector_Utils::NumberEncoding::BINARY);
    return {miniNum, microNum, nodeNum};
}

uint32_t HypersuccinctTree::leafSize(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<2>(node) > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMicroTree(node)) {
            isDummyAncestor = 1;
            Bitvector newMicroBit = getMicroDummyPointers(miniTree,std::get<1>(node));
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

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        //auto iter = entry.leaves.cbegin();
        //Bitvector entryLeafSize = Bitvector_Utils::getEntry(iter, std::get<2>(node), entry.leaves.cend(), Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA, {Bitvector_Utils::nullIterator()});
        Bitvector entryLeafSize = entry.leaves.at(std::get<2>(node));
        return res + Bitvector_Utils::decodeNumber(entryLeafSize, Bitvector_Utils::NumberEncoding::BINARY);
    }
    if(std::get<1>(node) > 0) {
        if(isDummyAncestorWithinMiniTree({std::get<0>(node),std::get<1>(node),0})) {
            uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            MiniTree newMiniTree = getMiniTree(newMini);
            res += Bitvector_Utils::decodeNumber(newMiniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
        }
        Bitvector microLeafSize = miniTree.microLeaves.at(std::get<1>(node));
        return res + Bitvector_Utils::decodeNumber(microLeafSize, Bitvector_Utils::NumberEncoding::BINARY);
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::leafRank(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));

    if(std::get<2>(node) > 0) {
        uint32_t dummyMarker = 0;
        if(isDummyAncestorWithinMicroTree({std::get<0>(node),std::get<1>(node),0})) {
            Bitvector dummy = getMicroDummys(miniTree,std::get<1>(node));
            uint32_t dummyNum = Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY);
            if(dummyNum == std::get<2>(node)) {
                dummyMarker = 1;
            }
            if(dummyNum < std::get<2>(node)) {
                Bitvector newMicroBit = getMicroDummyPointers(miniTree,std::get<1>(node));
                uint32_t newMicro = Bitvector_Utils::decodeNumber(newMicroBit, Bitvector_Utils::NumberEncoding::BINARY);
                Bitvector leaf_rank_node = miniTree.microRootLeafRanks.at(newMicro);
                res += Bitvector_Utils::decodeNumber(leaf_rank_node, Bitvector_Utils::NumberEncoding::BINARY) - 1;
            }
        }
        if(isDummyAncestorWithinMiniTree({std::get<0>(node),0,0})) {
            Bitvector dummy = getMiniDummy(std::get<0>(node));
            uint32_t dummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree, Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t dummyNum = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);
            if(dummyTree == std::get<1>(node) && dummyNum == std::get<2>(node)) {
                dummyMarker = 1;
            }
            if(dummyTree <= std::get<1>(node) && dummyNum < std::get<2>(node)) {
                uint32_t newMiniNum = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
                MiniTree newMini = getMiniTree(newMiniNum);
                res += Bitvector_Utils::decodeNumber(newMini.miniRootLeafRank,Bitvector_Utils::NumberEncoding::BINARY);
            }
        }

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        //auto iter = entry.leafRank.cbegin();
        //Bitvector leaf_rank_node = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.leafRank.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator()});
        Bitvector leaf_rank_node = entry.leafRank.at(std::get<2>(node));
        res += Bitvector_Utils::decodeNumber(leaf_rank_node, Bitvector_Utils::NumberEncoding::BINARY) - 1 - dummyMarker;
    }

    if(std::get<1>(node) > 0) {
        if(isDummyAncestorWithinMiniTree({std::get<0>(node),0,0})) {
            uint32_t dummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t dummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);
            if(dummyTree < std::get<1>(node) || (dummyTree == std::get<1>(node)&&dummyIndex < std::get<2>(node))) {
                uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
                MiniTree newMiniTree = getMiniTree(newMini);
                res += Bitvector_Utils::decodeNumber(newMiniTree.miniDummyLeafRank, Bitvector_Utils::NumberEncoding::BINARY);
            }
        }

        Bitvector leaf_rank_micro = miniTree.microRootLeafRanks.at(std::get<1>(node));
        res += (Bitvector_Utils::decodeNumber(leaf_rank_micro, Bitvector_Utils::NumberEncoding::BINARY) - 1);



        if (std::get<2>(node)!=0 && (std::get<1>(node) + std::get<2>(node) > 1)) {
            uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree, std::get<1>(node)).first;
            Bitvector fid = miniTree.FIDs.at(fidIndex);
            Bitvector tv = miniTree.typeVectors.at(fidIndex);
            std::pair<std::vector<uint32_t>, std::vector<uint32_t> > trees = getTreesForMicroFID(miniTree,
                                                                                                 std::get<1>(node));
            for (bool b: tv) {
                if (b) {
                    uint32_t tree = trees.first.front();
                    trees.first.erase(trees.first.cbegin());
                    if (tree == std::get<1>(node)) {
                        break;
                    } else {
                        res += leafSize({std::get<0>(node), tree, 0});
                    }
                } else {
                    uint32_t tree = trees.second.front();
                    trees.second.erase(trees.second.cbegin());
                    if (tree == std::get<1>(node)) {
                        break;
                    } else {
                        res += leafSize({std::get<0>(node), tree, 0});
                    }
                }
            }
        }

    }

    res += Bitvector_Utils::decodeNumber(miniTree.miniRootLeafRank, Bitvector_Utils::NumberEncoding::BINARY);

    //if(childRank(node) > 0) {
    if ((std::get<1>(node) != 0 || std::get<2>(node) != 0)) {
        uint32_t fidIndex = Bitvector_Utils::decodeNumber(miniTree.miniTopFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1;
        Bitvector fid = miniFIDs.at(fidIndex);
        Bitvector tv = miniTypeVectors.at(fidIndex);
        std::pair<std::vector<uint32_t>, std::vector<uint32_t> > trees = getTreesForFID(fidIndex);
        for (bool b: tv) {
            if (b) {
                uint32_t tree = trees.first.front();
                trees.first.erase(trees.first.cbegin());
                if (tree == std::get<0>(node)) {
                    break;
                } else {
                    res += leafSize({tree, 0, 0});
                }
            } else {
                uint32_t tree = trees.second.front();
                trees.second.erase(trees.second.cbegin());
                if (tree == std::get<0>(node)) {
                    break;
                } else {
                    res += leafSize({tree, 0, 0});
                }
            }
        }
    }
    //}


    return res;
}



HstNode HypersuccinctTree::levelAncestor(HstNode node, uint32_t level) {
    //TODO:
    if(std::get<2>(node) > 0) {
        //depth via lookputable
        uint32_t depth = 0;
        if(level <= depth) {
            //ancestormatrix oder so
            return {std::get<0>(node),std::get<1>(node),0};
        }
        return levelAncestor({std::get<0>(node), std::get<1>(node), 0}, level - depth);
    }
    if(std::get<1>(node) > 0) {
        //TreeSize or microTreeSize???
        uint32_t delta = ceil(sqrt(log2(Bitvector_Utils::decodeNumber(size, Bitvector_Utils::NumberEncoding::BINARY))));
        uint32_t depth = 0; //depth of MicroTree in MiniTree
        if(level > depth) {
            return levelAncestor({std::get<0>(node), 0, 0}, level - depth);
        }
        //???
    }
    //uint32_t delta =
    //M is miniTree Size?
    uint32_t delta = ceil(sqrt(Bitvector_Utils::decodeNumber(miniSize,Bitvector_Utils::NumberEncoding::BINARY)));
    return {};
}
