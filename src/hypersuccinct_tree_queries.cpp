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

uint32_t HypersuccinctTree::child_rank(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));
    if(std::get<0>(node) == 0 && std::get<1>(node) == 0 && std::get<2>(node) == 0) {
        return 0;
    }
    HstNode parent = getParent(node);
    MiniTree miniTreeParent = getMiniTree(std::get<0>(parent));
    if(Bitvector_Utils::decodeNumber(getMiniDummy(std::get<0>(parent)),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == std::get<1>(parent) && Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == std::get<2>(parent)) {
            return child_rank(parent);
        }
    }
    if(Bitvector_Utils::decodeNumber(getMicroDummys(miniTreeParent, std::get<1>(parent)),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        auto dummyIter = miniTree.dummys.cbegin();
        uint32_t dummyIndex = 0;
        for(int i=0; i<std::get<1>(node);i++) {
            Bitvector dummy = getMicroDummys(miniTree, i);
            if(Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                dummyIndex++;
            }
        }
        auto iterDummy = miniTree.microDummyPointers.cbegin();
        Bitvector newMicroBit = Bitvector_Utils::getEntry(iterDummy,dummyIndex,miniTree.microDummyPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
    }
    std::cout << std::get<0>(parent) << ":" << std::get<1>(parent) << ":" << std::get<2>(parent) << std::endl;
    if(std::get<1>(parent) == 0 && std::get<2>(parent)== 0) {
        std::pair<uint32_t ,uint32_t > fidIndices = convertTreeToFIDIndex(std::get<0>(parent));
        std::pair<std::vector<uint32_t >,std::vector<uint32_t> > parentTrees = getTreesForFID(std::get<0>(parent));
        if(!ListUtils::containsAny(parentTrees.second,{std::get<0>(parent)})) {

            auto iterD = miniFIDs.cbegin();
            Bitvector fid = Bitvector_Utils::getEntry(iterD, fidIndices.first, miniFIDs.cend(),
                                                      Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA,
                                                      {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1,
                                                       0});
            auto iterTV = miniTypeVectors.cbegin();
            auto iterF = miniFIDs.cbegin();
            Bitvector tv = Bitvector_Utils::getEntry(iterTV, fidIndices.first, miniTypeVectors.cend(),
                                                     Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX,
                                                     {iterF, miniFIDs.cend(), 2, 0});
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
        uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree, std::get<1>(parent)).first;
        auto iterD = miniTree.FIDs.cbegin();
        Bitvector fid = Bitvector_Utils::getEntry(iterD, fidIndex, miniTree.FIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1, 0});
        auto iterTV = miniTree.typeVectors.cbegin();
        auto iterF = miniTree.FIDs.cbegin();
        Bitvector tv = Bitvector_Utils::getEntry(iterTV, fidIndex, miniTree.typeVectors.cend(), Bitvector_Utils::BitvectorEncoding::VECTOR_INDEX, { iterF, miniTree.FIDs.cend(), 1, 0});
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
    std::cout << entry.childMatrix.size() << std::endl;
    std::cout << entry.ancestorMatrix.size() << std::endl;
    uint32_t size = sqrt(entry.childMatrix.size());
    Bitvector segment = Bitvector_Utils::getEntry(iter2, std::get<2>(parent), entry.childMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 0, size});
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
        Bitvector column = Bitvector_Utils::getEntry(iter, std::get<2>(node), entry.childMatrix.cend(), Bitvector_Utils::BitvectorEncoding::STATIC_MATRIX_COLUMN, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 0, size});
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
        uint32_t fidIndex = convertMicroTreeToFIDIndex(miniTree,std::get<1>(node)).second;
        if(fidIndex == -1) {
            if(std::get<1>(node) == 0) {
                return {-1,-1,-1};
            }
            for(int i=0; i< getMicroTreeCount(miniTree);i++) {
                Bitvector microDummy = getMicroDummys(miniTree,i);
                if(Bitvector_Utils::decodeNumber(microDummy, Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                    auto iterDummy = miniTree.microDummyPointers.cbegin();
                    Bitvector newMicroBit = Bitvector_Utils::getEntry(iterDummy, i,miniTree.microDummyPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
                    std::pair< std::vector<uint32_t> , std::vector<uint32_t >> trees = getTreesForMicroFID(miniTree,std::get<1>(node));
                    if(ListUtils::containsAny(trees.first,{Bitvector_Utils::decodeNumber(newMicroBit,Bitvector_Utils::NumberEncoding::BINARY)})) {
                        return {std::get<0>(node),i, Bitvector_Utils::decodeNumber(microDummy,Bitvector_Utils::NumberEncoding::BINARY)};
                    }
                }
            }
        }
        std::pair< std::vector<uint32_t >,std::vector<uint32_t > > trees = getTreesForMicroFID(miniTree, fidIndex);
        return {std::get<0>(node),trees.first.front(),0};
    }
    uint32_t fidIndex = convertTreeToFIDIndex(std::get<0>(node)).second;
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
            uint32_t microNum = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
            return degree({std::get<0>(node),microNum,0});
        }
        if(Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == std::get<1>(node) && Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == std::get<2>(node) ) {
            uint32_t miniPoint = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            return degree({miniPoint,0,0});
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
    uint32_t fidID = convertTreeToFIDIndex(std::get<0>(node)).first;
    auto iterD = miniFIDs.cbegin();
    Bitvector fid = Bitvector_Utils::getEntry(iterD, fidID, miniFIDs.cend(), Bitvector_Utils::BitvectorEncoding::ELIAS_GAMMA, {Bitvector_Utils::nullIterator, Bitvector_Utils::nullIterator, 1, 0});
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
                Bitvector dummy = getMicroDummys(miniTree, i);
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

uint32_t HypersuccinctTree::leaf_rank(HstNode node) {
    //TODO: FID Check for Child Rank (See 0,1,0)
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(std::get<0>(node));

    /*Bitvector fid = getFIDforMiniTree(std::get<0>(node));
    uint32_t fidIndex = 0;
    std::tuple<std::vector<uint32_t >,std::vector<uint32_t >> trees = getTreesForFID(fidIndex);
    if()*/

    if(std::get<2>(node) > 0) {
        if(isDummyAncestorWithinMicroTree({std::get<0>(node),std::get<1>(node),0})) {
            Bitvector dummy = getMicroDummys(miniTree,std::get<1>(node));
            uint32_t dummyNum = Bitvector_Utils::decodeNumber(dummy,Bitvector_Utils::NumberEncoding::BINARY);
            if(dummyNum < std::get<2>(node)) {
                //isDummyAncestor = 1;
                auto dummyIter = miniTree.dummys.cbegin();
                uint32_t dummyIndex = 0;
                for (int i = 0; i < std::get<1>(node); i++) {
                    Bitvector dummyS = getMicroDummys(miniTree, i);
                    if (Bitvector_Utils::decodeNumber(dummyS, Bitvector_Utils::NumberEncoding::BINARY) != 0) {
                        dummyIndex++;
                    }
                }
                auto iterDummy = miniTree.microDummyPointers.cbegin();
                Bitvector newMicroBit = Bitvector_Utils::getEntry(iterDummy, dummyIndex,miniTree.microDummyPointers.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
                uint32_t newMicro = Bitvector_Utils::decodeNumber(newMicroBit, Bitvector_Utils::NumberEncoding::BINARY);
                auto iterm = miniTree.microRootLeafRanks.cbegin();
                Bitvector leaf_rank_node = Bitvector_Utils::getEntry(iterm, newMicro,miniTree.microRootLeafRanks.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
                res += Bitvector_Utils::decodeNumber(leaf_rank_node, Bitvector_Utils::NumberEncoding::BINARY) - 1;
            }
        }
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,std::get<1>(node)));
        auto iter = entry.leafRank.cbegin();
        Bitvector leaf_rank_node = Bitvector_Utils::getEntry(iter,std::get<2>(node),entry.leafRank.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        res += Bitvector_Utils::decodeNumber(leaf_rank_node, Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    //if(std::get<1>(node) > 0) {
        if(isDummyAncestorWithinMiniTree({std::get<0>(node),0,0})) {
            uint32_t dummyTree = Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY);
            uint32_t dummyIndex = Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY);
            if(dummyTree < std::get<1>(node) || (dummyTree == std::get<1>(node)&&dummyIndex < std::get<2>(node))) {
                uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
                MiniTree newMiniTree = getMiniTree(newMini);
                res += Bitvector_Utils::decodeNumber(newMiniTree.miniDummyLeafRank, Bitvector_Utils::NumberEncoding::BINARY);
            }
        }

        auto iter = miniTree.microRootLeafRanks.cbegin();
        Bitvector leaf_rank_micro = Bitvector_Utils::getEntry(iter,std::get<1>(node),miniTree.microRootLeafRanks.cend(),Bitvector_Utils::BitvectorEncoding::PURE_ELIAS_GAMMA,{Bitvector_Utils::nullIterator});
        res += (Bitvector_Utils::decodeNumber(leaf_rank_micro, Bitvector_Utils::NumberEncoding::BINARY) - 1);
    //}
    return res + Bitvector_Utils::decodeNumber(miniTree.miniRootLeafRank, Bitvector_Utils::NumberEncoding::BINARY);
}



HstNode HypersuccinctTree::levelAncestor(uint32_t level, HstNode node) {
    //TODO:
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
