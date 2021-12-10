//
// Created by Christopher Pack on 20.05.2021.
//

#include "pht/hypersuccinct_tree.h"
#include <iostream>

using namespace pht;

bool HypersuccinctTree::isDummyAncestorWithinMiniTree(HstNode node) {
    MiniTree mini = getMiniTree(node.mini);
    if(mini.rootAncestors.empty()) {
        return false;
    }
    HstNode dummy = {node.mini, BitvectorUtils::decodeNumber(mini.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY) , BitvectorUtils::decodeNumber(mini.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY)};
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
}

bool HypersuccinctTree::isDummyAncestorWithinMicroTree(HstNode node) {
    MiniTree mini = getMiniTree(node.mini);
    Bitvector dummy = getMicroDummys(mini,node.micro);
    uint32_t dummyNum = BitvectorUtils::decodeNumber(dummy,BitvectorUtils::NumberEncoding::BINARY);
    if(dummyNum == 0) {
        return false;
    }
    return lookupTableAncestorMatrixComparison(getLookupTableEntry(getMicroTree(mini, node.micro)),
                                               node.node, dummyNum);
}

HstNode HypersuccinctTree::child(HstNode parent, uint32_t index) {
    uint32_t miniRes = 0;
    uint32_t microRes = 0;
    uint32_t nodeRes = 0;
    uint32_t microIndexHelp = 0;
    uint32_t nodeIndexHelp = 0;
    bool checkMini = true;
    bool checkMicro = false;
    bool checkNode = false;
    MiniTree miniTreeParent = getMiniTree(parent.mini);
    if(degree(parent) < index) {
        return {};
    }

    //Checks if given Node is a dummy
    if(BitvectorUtils::decodeNumber(miniDummys.at(parent.mini),BitvectorUtils::NumberEncoding::BINARY) != 0) {
        if(BitvectorUtils::decodeNumber(miniTreeParent.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY) == parent.micro && BitvectorUtils::decodeNumber(miniTreeParent.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY) == parent.node ) {
            return child({BitvectorUtils::decodeNumber(miniTreeParent.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY), 0, 0}, index);
        }
    }
    if(parent.node != 0) {
        if(BitvectorUtils::decodeNumber(miniTreeParent.dummys.at(parent.micro),BitvectorUtils::NumberEncoding::BINARY) == parent.node) {
            return child({parent.mini, BitvectorUtils::decodeNumber(miniTreeParent.microDummyPointers.at(parent.micro),BitvectorUtils::NumberEncoding::BINARY), 0},index);
        }
    }

    if(parent.node > 0) {
        miniRes = parent.mini;
        microRes = parent.micro;
        nodeIndexHelp = index;
        checkMini = false;
        checkNode = true;
    } else if(parent.micro > 0) {
        miniRes = parent.mini;
        microIndexHelp = index;
        checkMini = false;
        checkMicro = true;
    }

    if(checkMini) {
        uint32_t miniFIDIndex = BitvectorUtils::decodeNumber(miniTreeParent.miniTopFIDIndex, BitvectorUtils::NumberEncoding::BINARY) - 1;
        uint32_t firstLowMini = BitvectorUtils::decodeNumber(miniFIDLowTree.at(miniFIDIndex), BitvectorUtils::NumberEncoding::BINARY) - 1;
        uint32_t firstTopMini = BitvectorUtils::decodeNumber(miniFIDTopTree.at(miniFIDIndex), BitvectorUtils::NumberEncoding::BINARY) - 1;
        succinct_bv::BitVector miniTVSupport = miniTypeVectorsSupport.at(miniFIDIndex);
        succinct_bv::BitVector miniFIDSupport = miniFIDsSupport.at(miniFIDIndex);
        auto fidRank = static_cast<uint32_t>(miniFIDSupport.Rank(index));
        uint32_t fidRankSelect = 0;
        if (fidRank != 0) {
            fidRankSelect = static_cast<uint32_t>(miniFIDSupport.Select(fidRank - 1));
        }
        auto tvRank = static_cast<uint32_t>(miniTVSupport.Rank(fidRank - 1));
        //Illegal value is 0 -> lower than any fidRank (since first entry in FID is always 1)
        uint32_t tvRankSelect = 0;
        if (tvRank != 0) {
            tvRankSelect = static_cast<uint32_t>(miniTVSupport.Select(tvRank - 1) + 1);
        }
        if (fidRankSelect != index) {
            checkMicro = true;
            microIndexHelp = index - fidRankSelect;
        }
        if (tvRankSelect == fidRank) {
            miniRes = firstTopMini + (tvRank - 1);
            checkMicro = true;
        } else {
            if(index > 0 && fidRank == miniFIDSupport.Rank(index - 1)) {
                miniRes = firstTopMini + (tvRank - 1);
            } else {
                uint32_t resFIDLocalIndex = (fidRank - 1) - tvRank;
                MiniTree firstLowMiniTree = getMiniTree(firstLowMini);
                uint32_t firstLowMiniFID = BitvectorUtils::decodeNumber(firstLowMiniTree.miniTopFIDIndex, BitvectorUtils::NumberEncoding::BINARY) - 1;
                uint32_t resFIDIndex = firstLowMiniFID + resFIDLocalIndex;
                miniRes = BitvectorUtils::decodeNumber(miniFIDTopTree.at(resFIDIndex), BitvectorUtils::NumberEncoding::BINARY) - 1;
            }
        }
        miniTreeParent = getMiniTree(miniRes);
    }

    if(checkMicro) {
        uint32_t microFIDIndex = BitvectorUtils::decodeNumber(miniTreeParent.microTopFIDIndices.at(parent.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
        uint32_t firstLowMicro = BitvectorUtils::decodeNumber(miniTreeParent.microFIDLowTrees.at(microFIDIndex),BitvectorUtils::NumberEncoding::BINARY) - 1;
        uint32_t firstTopMicro = BitvectorUtils::decodeNumber(miniTreeParent.microFIDTopTrees.at(microFIDIndex),BitvectorUtils::NumberEncoding::BINARY) - 1;
        succinct_bv::BitVector microTVSupport = miniTreeParent.typeVectorsSupport.at(microFIDIndex);
        succinct_bv::BitVector microFIDSupport = miniTreeParent.FIDsSupport.at(microFIDIndex);
        auto fidRank = static_cast<uint32_t>(microFIDSupport.Rank(microIndexHelp));
        uint32_t fidRankSelect = 0;
        if(fidRank != 0) {
            fidRankSelect = static_cast<uint32_t>(microFIDSupport.Select(fidRank - 1));
        }
        auto tvRank = static_cast<uint32_t>(microTVSupport.Rank(fidRank - 1));
        uint32_t tvRankSelect = 0;
        if(tvRank != 0) {
            tvRankSelect = static_cast<uint32_t>(microTVSupport.Select(tvRank - 1) + 1);
        }
        if(fidRankSelect != microIndexHelp) {
            checkNode = true;
            nodeIndexHelp = microIndexHelp - fidRankSelect;
        }
        if(tvRankSelect == fidRank) {
            microRes = firstTopMicro + (tvRank - 1);
            checkNode = true;
        } else {
            if(microIndexHelp > 0 && fidRank == microFIDSupport.Rank(microIndexHelp - 1)) {
                microRes = firstTopMicro + (tvRank - 1);
            } else {

                uint32_t resFIDLocalIndex = (fidRank - 1) - tvRank;
                uint32_t firstLowMicroFID = BitvectorUtils::decodeNumber(miniTreeParent.microTopFIDIndices.at(firstLowMicro), BitvectorUtils::NumberEncoding::BINARY) - 1;
                uint32_t resFIDIndex = firstLowMicroFID + resFIDLocalIndex;
                microRes = BitvectorUtils::decodeNumber(miniTreeParent.microFIDTopTrees.at(resFIDIndex), BitvectorUtils::NumberEncoding::BINARY) - 1;
            }
        }
    }

    if(checkNode) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(getMiniTree(miniRes),microRes));
        auto matSize = static_cast<uint32_t>(sqrt(entry.childMatrix.size()));
        auto startRank = static_cast<uint32_t>(entry.childMatrixSupport.Rank(matSize * parent.node));
        auto startRankSelect = static_cast<uint32_t>(entry.childMatrixSupport.Select(startRank + nodeIndexHelp));
        nodeRes = startRankSelect % matSize;
        if(startRankSelect > (matSize * (parent.node + 1))) {
            return {};
        }
        if(nodeRes == parent.node) {
            throw std::runtime_error("HypersuccinctTree: Child Determination Error");
        }
    }

    //Checks if Result is a dummy
    MiniTree miniTreeRes = getMiniTree(miniRes);
    if(BitvectorUtils::decodeNumber(miniDummys.at(miniRes),BitvectorUtils::NumberEncoding::BINARY) != 0) {
        if(BitvectorUtils::decodeNumber(miniTreeRes.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY) == microRes && BitvectorUtils::decodeNumber(miniTreeRes.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY) == nodeRes ) {
            return {BitvectorUtils::decodeNumber(miniTreeRes.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY), 0, 0};
        }
    }
    if(nodeRes != 0) {
        if(BitvectorUtils::decodeNumber(miniTreeRes.dummys.at(microRes),BitvectorUtils::NumberEncoding::BINARY) == nodeRes) {
            return {miniRes, BitvectorUtils::decodeNumber(miniTreeRes.microDummyPointers.at(microRes),BitvectorUtils::NumberEncoding::BINARY), 0};
        }
    }

    return {miniRes,microRes,nodeRes};
}

uint32_t HypersuccinctTree::childRank(HstNode node) {
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.mini == 0 && node.micro == 0 && node.node == 0) {
        return 0;
    }
    HstNode parent = getParentForQuery(node);
    MiniTree miniTreeParent = getMiniTree(parent.mini);
    uint32_t res = 0;

    //If given node is child of a dummy, use ChildRank of dummy instead
    if(BitvectorUtils::decodeNumber(miniDummys.at(parent.mini),BitvectorUtils::NumberEncoding::BINARY) != 0) {
        if(BitvectorUtils::decodeNumber(miniTreeParent.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY) == parent.micro && BitvectorUtils::decodeNumber(miniTreeParent.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY) == parent.node ) {
            return childRank(parent);
        }
    }
    if(parent.node != 0) {
        if(BitvectorUtils::decodeNumber(miniTreeParent.dummys.at(parent.micro),BitvectorUtils::NumberEncoding::BINARY) == parent.node) {
            return childRank(parent);
        }
    }

    if(node.node > 0) {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTreeParent, parent.micro));
            auto matSize = static_cast<uint32_t>(sqrt(entry.childMatrix.size()));
            auto startRank = static_cast<uint32_t>(entry.childMatrixSupport.Rank(matSize * parent.node));
            auto fullRank = static_cast<uint32_t>(entry.childMatrixSupport.Rank(matSize * parent.node + node.node));

        if(parent.node > 0) {
            return fullRank - startRank - 1;
        } else {
            res += fullRank - startRank - 1;
        }
    }

    if(node.micro > 0) {
        if (node.node!=0) {
            return res + BitvectorUtils::decodeNumber(miniTree.microExtendedChildRanks.at(node.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
        } else {
            res += BitvectorUtils::decodeNumber(miniTree.microChildRanks.at(node.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
        }

    }

    if(parent.mini == node.mini) {
        uint32_t microFIDIndex = BitvectorUtils::decodeNumber(miniTreeParent.microTopFIDIndices.at(parent.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
        uint32_t firstMicro = BitvectorUtils::decodeNumber(miniTreeParent.microFIDTopTrees.at(microFIDIndex),BitvectorUtils::NumberEncoding::BINARY) - 1;
        if (miniTreeParent.typeVectorsSupport.at(microFIDIndex).Rank(0) == 0) {
            firstMicro = BitvectorUtils::decodeNumber(miniTreeParent.microFIDLowTrees.at(microFIDIndex),BitvectorUtils::NumberEncoding::BINARY) - 1;
        }
        uint32_t miniFIDIndex =BitvectorUtils::decodeNumber(miniTreeParent.miniTopFIDIndex, BitvectorUtils::NumberEncoding::BINARY) -1;
        uint32_t firstMini = BitvectorUtils::decodeNumber(miniFIDTopTree.at(miniFIDIndex), BitvectorUtils::NumberEncoding::BINARY) - 1;
        if (miniTypeVectorsSupport.at(miniFIDIndex).Rank(0) == 0) {
            firstMini = BitvectorUtils::decodeNumber(miniFIDLowTree.at(miniFIDIndex), BitvectorUtils::NumberEncoding::BINARY) - 1;
        }

        if(node.mini != firstMini) {
            return res + BitvectorUtils::decodeNumber(miniTree.microChildRanks.at(0),BitvectorUtils::NumberEncoding::BINARY) - 1;
        }
        else if(node.micro==0 && node.micro != firstMicro) {
            res += BitvectorUtils::decodeNumber(miniTree.microExtendedChildRanks.at(node.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
        }
    } else {

        res += BitvectorUtils::decodeNumber(miniTree.miniChildRank, BitvectorUtils::NumberEncoding::BINARY);
    }
    return res;
}

HstNode HypersuccinctTree::getParentForQuery(HstNode node) {
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        Bitvector parentBit = entry.parentPointers.at(node.node);
        uint32_t parent = BitvectorUtils::decodeNumber(parentBit,BitvectorUtils::NumberEncoding::BINARY);
        return {node.mini,node.micro,parent - 1};
    }
    if(node.micro > 0) {
        Bitvector parentBit = miniTree.microParents.at(node.micro);
        uint32_t parentMicro = BitvectorUtils::decodeNumber(parentBit,BitvectorUtils::NumberEncoding::BINARY) - 1;
        uint32_t parentDummy = BitvectorUtils::decodeNumber(miniTree.dummys.at(parentMicro),BitvectorUtils::NumberEncoding::BINARY);
        uint32_t parentFID = BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(node.micro),BitvectorUtils::NumberEncoding::BINARY);
        if(parentFID == 0) {
            return {node.mini,parentMicro,parentDummy};
        }
        return {node.mini,parentMicro,0};
    }
    uint32_t parentMini = BitvectorUtils::decodeNumber(miniTree.miniParent,BitvectorUtils::NumberEncoding::BINARY) - 1;
    MiniTree parentMiniTree = getMiniTree(parentMini);
    uint32_t parentFID = BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,BitvectorUtils::NumberEncoding::BINARY);
    if(parentFID == 0) {
        return {parentMini,BitvectorUtils::decodeNumber(parentMiniTree.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY),BitvectorUtils::decodeNumber(parentMiniTree.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY)};
    }
    return {parentMini,0,0};
}

HstNode HypersuccinctTree::getParent(HstNode node) {
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        Bitvector parentBit = entry.parentPointers.at(node.node);
        uint32_t parent = BitvectorUtils::decodeNumber(parentBit,BitvectorUtils::NumberEncoding::BINARY) - 1;
        return {node.mini,node.micro,parent};
    }
    if(node.micro > 0) {
        Bitvector parentBit = miniTree.microParents.at(node.micro);
        uint32_t parentMicro = BitvectorUtils::decodeNumber(parentBit,BitvectorUtils::NumberEncoding::BINARY) - 1;
        uint32_t parentDummy = BitvectorUtils::decodeNumber(miniTree.dummys.at(parentMicro),BitvectorUtils::NumberEncoding::BINARY);
        uint32_t parentFID = BitvectorUtils::decodeNumber(miniTree.microLowFIDIndices.at(node.micro),BitvectorUtils::NumberEncoding::BINARY);
        if(parentFID == 0) {
            return getParent({node.mini,parentMicro,parentDummy});
        }
        return {node.mini,parentMicro,0};
    }
    uint32_t parentMini = BitvectorUtils::decodeNumber(miniTree.miniParent,BitvectorUtils::NumberEncoding::BINARY) - 1;
    MiniTree parentMiniTree = getMiniTree(parentMini);
    uint32_t parentFID = BitvectorUtils::decodeNumber(miniTree.miniLowFIDIndex,BitvectorUtils::NumberEncoding::BINARY);
    if(parentFID == 0) {
        return getParent({parentMini,BitvectorUtils::decodeNumber(parentMiniTree.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY),BitvectorUtils::decodeNumber(parentMiniTree.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY)});
    }
    return {parentMini,0,0};
}

uint32_t HypersuccinctTree::degree(HstNode node) {
    if(node.node > 0) {
        MiniTree miniTree = getMiniTree(node.mini);
        if(BitvectorUtils::decodeNumber(getMicroDummys(miniTree,node.micro),BitvectorUtils::NumberEncoding::BINARY) == node.node) {
            Bitvector subTreeB = getMicroDummyPointers(miniTree,node.micro);
            uint32_t microNum = BitvectorUtils::decodeNumber(subTreeB,BitvectorUtils::NumberEncoding::BINARY);
            return degree({node.mini,microNum,0});
        }
        if(BitvectorUtils::decodeNumber(miniTree.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY) == node.micro && BitvectorUtils::decodeNumber(miniTree.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY) == node.node ) {
            uint32_t miniPoint = BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY);
            return degree({miniPoint,0,0});
        }
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        Bitvector degreeB = entry.degree.at(node.node);
        return BitvectorUtils::decodeNumber(degreeB,BitvectorUtils::NumberEncoding::BINARY) - 1;
    }
    if(node.micro > 0) {
        MiniTree miniTree = getMiniTree(node.mini);
        uint32_t fidIndex = BitvectorUtils::decodeNumber(miniTree.microTopFIDIndices.at(node.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
        return static_cast<uint32_t>(miniTree.FIDs.at(fidIndex).size());
    }
    uint32_t fidID = BitvectorUtils::decodeNumber(getMiniTree(node.mini).miniTopFIDIndex,BitvectorUtils::NumberEncoding::BINARY)-1;
    Bitvector fid = miniFIDs.at(fidID);
    return static_cast<uint32_t>(fid.size());
}

uint32_t HypersuccinctTree::subtreeSize(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMicroTree(node)) {
            isDummyAncestor++;
            Bitvector subTreeB = getMicroDummyPointers(miniTree,node.micro);
            uint32_t dummyPointer = BitvectorUtils::decodeNumber(subTreeB,BitvectorUtils::NumberEncoding::BINARY);
            Bitvector subtreeSize = miniTree.microSubTrees.at(dummyPointer);
            res += BitvectorUtils::decodeNumber(subtreeSize, BitvectorUtils::NumberEncoding::BINARY);
        }
        if(isDummyAncestorWithinMiniTree(node)) {
            isDummyAncestor++;
            uint32_t dummyPointer = BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY);
            res += BitvectorUtils::decodeNumber(getMiniTree(dummyPointer).subTree, BitvectorUtils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        Bitvector degreeB = entry.subTrees.at(node.node);
        return res + BitvectorUtils::decodeNumber(degreeB,BitvectorUtils::NumberEncoding::BINARY) - 1;
    }
    if(node.micro > 0) {
        if(isDummyAncestorWithinMiniTree(node)) {
            uint32_t dummyPointer = BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY);
            res += BitvectorUtils::decodeNumber(getMiniTree(dummyPointer).subTree, BitvectorUtils::NumberEncoding::BINARY);
        }
        Bitvector subTreeB = miniTree.microSubTrees.at(node.micro);
        return res + BitvectorUtils::decodeNumber(subTreeB,BitvectorUtils::NumberEncoding::BINARY);
    }
    return res + BitvectorUtils::decodeNumber(miniTree.subTree,BitvectorUtils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::depth(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        Bitvector entryDepth = entry.nodeDepths.at(node.node);
        res += BitvectorUtils::decodeNumber(entryDepth, BitvectorUtils::NumberEncoding::BINARY) - 1;
    }
    if(node.micro > 0) {
        Bitvector microDepth = miniTree.rootDepths.at(node.micro);
        res += BitvectorUtils::decodeNumber(microDepth, BitvectorUtils::NumberEncoding::BINARY) - 1;
    }
    return res + BitvectorUtils::decodeNumber(miniTree.miniDepth, BitvectorUtils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::height(HstNode node) {
    uint32_t res = 0;
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        if(isDummyAncestorWithinMicroTree(node) || isDummyAncestorWithinMiniTree(node)) {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
            Bitvector entryDepth = entry.nodeDepths.at(node.node);
            res -= (BitvectorUtils::decodeNumber(entryDepth, BitvectorUtils::NumberEncoding::BINARY) - 1);
        }
        else {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
            Bitvector entryDepth = entry.nodeHeights.at(node.node);
            return res + BitvectorUtils::decodeNumber(entryDepth, BitvectorUtils::NumberEncoding::BINARY) - 1;
        }
    }
    if(node.micro > 0) {
        if(isDummyAncestorWithinMiniTree({node.mini,node.micro,0})) {
            Bitvector microDepth = miniTree.rootDepths.at(node.micro);
            res -= (BitvectorUtils::decodeNumber(microDepth, BitvectorUtils::NumberEncoding::BINARY) - 1);
        }
        else {
            Bitvector microDepth = miniTree.rootHeights.at(node.micro);
            return res + BitvectorUtils::decodeNumber(microDepth, BitvectorUtils::NumberEncoding::BINARY) - 1;
        }
    }
    return res + BitvectorUtils::decodeNumber(miniTree.miniHeight, BitvectorUtils::NumberEncoding::BINARY);
}

HstNode HypersuccinctTree::leftmostLeaf(HstNode node) {
    uint32_t miniNum = node.mini;
    uint32_t microNum = node.micro;
    uint32_t nodeNum = node.node;
    MiniTree miniTree = getMiniTree(node.mini);
    MiniTree leftmostMini = miniTree;
    uint32_t miniDummyTree = BitvectorUtils::decodeNumber(miniTree.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY);
    uint32_t miniDummyIndex = BitvectorUtils::decodeNumber(miniTree.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY);

    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        Bitvector entryleftmost = entry.leftmost_leaf.at(nodeNum);
        nodeNum = BitvectorUtils::decodeNumber(entryleftmost, BitvectorUtils::NumberEncoding::BINARY);
        Bitvector microDummy = getMicroDummys(miniTree, microNum);
        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY));
        }
        else if (nodeNum == BitvectorUtils::decodeNumber(microDummy, BitvectorUtils::NumberEncoding::BINARY)) {
            Bitvector subTreeB = getMicroDummyPointers(miniTree,node.micro);
            microNum = BitvectorUtils::decodeNumber(subTreeB,BitvectorUtils::NumberEncoding::BINARY);
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }

    if((node.micro > 0 || node.node > 0)  && (microNum != miniDummyTree || nodeNum != miniDummyIndex)) {
        Bitvector leftmostMicro = leftmostMini.microTreeLeftmostLeafPointers.at(microNum);
        microNum = BitvectorUtils::decodeNumber(leftmostMicro,BitvectorUtils::NumberEncoding::BINARY);
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        Bitvector entryleftmost = entry.leftmost_leaf.at(0);
        nodeNum = BitvectorUtils::decodeNumber(entryleftmost, BitvectorUtils::NumberEncoding::BINARY);

        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY));
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }
    miniNum = BitvectorUtils::decodeNumber(miniTree.miniTreeLeftmostLeafPointer,BitvectorUtils::NumberEncoding::BINARY);
    leftmostMini = getMiniTree(miniNum);
    Bitvector leftmostMicro = leftmostMini.microTreeLeftmostLeafPointers.at(0);
    microNum = BitvectorUtils::decodeNumber(leftmostMicro,BitvectorUtils::NumberEncoding::BINARY);
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini,microNum));
    Bitvector entryleftmost = entry.leftmost_leaf.at(0);
    nodeNum = BitvectorUtils::decodeNumber(entryleftmost,BitvectorUtils::NumberEncoding::BINARY);
    return {miniNum, microNum, nodeNum};
}

HstNode HypersuccinctTree::rightmostLeaf(HstNode node) {
    uint32_t miniNum = node.mini;
    uint32_t microNum = node.micro;
    uint32_t nodeNum = node.node;
    MiniTree miniTree = getMiniTree(node.mini);
    MiniTree leftmostMini = miniTree;
    uint32_t miniDummyTree = BitvectorUtils::decodeNumber(miniTree.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY);
    uint32_t miniDummyIndex = BitvectorUtils::decodeNumber(miniTree.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY);

    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        Bitvector entryleftmost = entry.rightmost_leaf.at(nodeNum);
        nodeNum = BitvectorUtils::decodeNumber(entryleftmost, BitvectorUtils::NumberEncoding::BINARY);
        Bitvector microDummy = getMicroDummys(miniTree, microNum);
        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY));
        }
        else if (nodeNum == BitvectorUtils::decodeNumber(microDummy, BitvectorUtils::NumberEncoding::BINARY)) {
            Bitvector subTreeB = getMicroDummyPointers(miniTree,node.micro);
            microNum = BitvectorUtils::decodeNumber(subTreeB,BitvectorUtils::NumberEncoding::BINARY);
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }

    if((node.micro > 0 || node.node > 0) && (microNum != miniDummyTree || nodeNum != miniDummyIndex)) {
        Bitvector leftmostMicro = leftmostMini.microTreeRightmostLeafPointers.at(microNum);
        microNum = BitvectorUtils::decodeNumber(leftmostMicro,BitvectorUtils::NumberEncoding::BINARY);
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini, microNum));
        Bitvector entryleftmost = entry.rightmost_leaf.at(0);
        nodeNum = BitvectorUtils::decodeNumber(entryleftmost, BitvectorUtils::NumberEncoding::BINARY);

        if(microNum == miniDummyTree && nodeNum == miniDummyIndex) {
            miniTree = getMiniTree(BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY));
        }
        else {
            return {miniNum, microNum, nodeNum};
        }
    }
    miniNum = BitvectorUtils::decodeNumber(miniTree.miniTreeRightmostLeafPointer,BitvectorUtils::NumberEncoding::BINARY);
    leftmostMini = getMiniTree(miniNum);
    Bitvector leftmostMicro = leftmostMini.microTreeRightmostLeafPointers.at(0);
    microNum = BitvectorUtils::decodeNumber(leftmostMicro,BitvectorUtils::NumberEncoding::BINARY);
    LookupTableEntry entry = getLookupTableEntry(getMicroTree(leftmostMini,microNum));
    Bitvector entryleftmost = entry.rightmost_leaf.at(0);
    nodeNum = BitvectorUtils::decodeNumber(entryleftmost,BitvectorUtils::NumberEncoding::BINARY);
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
            uint32_t newMicro = BitvectorUtils::decodeNumber(newMicroBit,BitvectorUtils::NumberEncoding::BINARY);
            Bitvector microLeafSize = miniTree.microLeaves.at(newMicro);
            res += BitvectorUtils::decodeNumber(microLeafSize, BitvectorUtils::NumberEncoding::BINARY);

        }

        if (isDummyAncestorWithinMiniTree(node)) {
            isDummyAncestor = 1;
            uint32_t newMini = BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY);
            MiniTree newMiniTree = getMiniTree(newMini);
            res += BitvectorUtils::decodeNumber(newMiniTree.miniLeaves, BitvectorUtils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        Bitvector entryLeafSize = entry.leaves.at(node.node);
        return res + BitvectorUtils::decodeNumber(entryLeafSize, BitvectorUtils::NumberEncoding::BINARY);
    }
    if(node.micro > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMiniTree({node.mini,node.micro,0})) {
            isDummyAncestor = 1;
            uint32_t newMini = BitvectorUtils::decodeNumber(miniTree.miniDummyPointer,BitvectorUtils::NumberEncoding::BINARY);
            MiniTree newMiniTree = getMiniTree(newMini);
            res += BitvectorUtils::decodeNumber(newMiniTree.miniLeaves, BitvectorUtils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;
        Bitvector microLeafSize = miniTree.microLeaves.at(node.micro);
        return res + BitvectorUtils::decodeNumber(microLeafSize, BitvectorUtils::NumberEncoding::BINARY);
    }
    return res + BitvectorUtils::decodeNumber(miniTree.miniLeaves, BitvectorUtils::NumberEncoding::BINARY);
}

uint32_t HypersuccinctTree::leafRank(HstNode node) {
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.mini == 0 && node.micro == 0 && node.node == 0) {
        return 0;
    }

    HstNode parent = getParentForQuery(node);
    MiniTree miniTreeParent = getMiniTree(parent.mini);
    uint32_t res = 0;

    //If given node is child of a dummy, use LeafRank of dummy instead
    if(BitvectorUtils::decodeNumber(miniDummys.at(parent.mini),BitvectorUtils::NumberEncoding::BINARY) != 0) {
        if(BitvectorUtils::decodeNumber(miniTreeParent.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY) == parent.micro && BitvectorUtils::decodeNumber(miniTreeParent.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY) == parent.node ) {
            return leafRank(parent);
        }
    }
    if(parent.node != 0) {
        if(BitvectorUtils::decodeNumber(miniTreeParent.dummys.at(parent.micro),BitvectorUtils::NumberEncoding::BINARY) == parent.node) {
            return leafRank(parent);
        }
    }

    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTreeParent, parent.micro));
        if(BitvectorUtils::decodeNumber(miniDummys.at(node.mini),BitvectorUtils::NumberEncoding::BINARY) != 0) {
            if(BitvectorUtils::decodeNumber(miniTree.miniDummyTree,BitvectorUtils::NumberEncoding::BINARY) == node.micro && BitvectorUtils::decodeNumber(miniTree.miniDummyIndex,BitvectorUtils::NumberEncoding::BINARY) == node.node ) {
                res--;
            }
        }
        if(node.node != 0) {
            if(BitvectorUtils::decodeNumber(miniTree.dummys.at(node.micro),BitvectorUtils::NumberEncoding::BINARY) == node.node) {
                res--;
            }
        }
        res += BitvectorUtils::decodeNumber(entry.leafRank.at(node.node),BitvectorUtils::NumberEncoding::BINARY) - 1;
    }

    if(node.micro > 0) {
        if (node.node!=0) {
            res += BitvectorUtils::decodeNumber(miniTree.microExtendedLeafRanks.at(node.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
        } else {
            res += BitvectorUtils::decodeNumber(miniTree.microRootLeafRanks.at(node.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
        }

    }
    if(parent.mini != node.mini) {
        return res + BitvectorUtils::decodeNumber(miniTree.miniRootLeafRank, BitvectorUtils::NumberEncoding::BINARY);
    }
    uint32_t microFIDIndex = BitvectorUtils::decodeNumber(miniTreeParent.microTopFIDIndices.at(parent.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
    uint32_t firstMicro = BitvectorUtils::decodeNumber(miniTreeParent.microFIDTopTrees.at(microFIDIndex),BitvectorUtils::NumberEncoding::BINARY) - 1;
    if (miniTreeParent.typeVectorsSupport.at(microFIDIndex).Rank(0) == 0) {
        firstMicro = BitvectorUtils::decodeNumber(miniTreeParent.microFIDLowTrees.at(microFIDIndex),BitvectorUtils::NumberEncoding::BINARY) - 1;
    }
    uint32_t miniFIDIndex =BitvectorUtils::decodeNumber(miniTreeParent.miniTopFIDIndex, BitvectorUtils::NumberEncoding::BINARY) -1;
    uint32_t firstMini = BitvectorUtils::decodeNumber(miniFIDTopTree.at(miniFIDIndex), BitvectorUtils::NumberEncoding::BINARY) - 1;
    if (miniTypeVectorsSupport.at(miniFIDIndex).Rank(0) == 0) {
        firstMini = BitvectorUtils::decodeNumber(miniFIDLowTree.at(miniFIDIndex), BitvectorUtils::NumberEncoding::BINARY) - 1;
    }

    if(node.mini != firstMini) {
        return res + BitvectorUtils::decodeNumber(miniTree.microRootLeafRanks.at(0),BitvectorUtils::NumberEncoding::BINARY) - 1;
    }
    else if(node.micro==0 && node.micro != firstMicro) {
        res += BitvectorUtils::decodeNumber(miniTree.microExtendedLeafRanks.at(node.micro),BitvectorUtils::NumberEncoding::BINARY) - 1;
    }
    res += BitvectorUtils::decodeNumber(miniTree.miniRootLeafRank, BitvectorUtils::NumberEncoding::BINARY);

    return res;
}
