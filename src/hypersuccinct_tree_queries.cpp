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
    if(Bitvector_Utils::decodeNumber(miniDummys.at(parent.mini),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == parent.micro && Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == parent.node ) {
            return child({Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY), 0, 0}, index);
        }
    }
    if(parent.node != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.dummys.at(parent.micro),Bitvector_Utils::NumberEncoding::BINARY) == parent.node) {
            return child({parent.mini, Bitvector_Utils::decodeNumber(miniTreeParent.microDummyPointers.at(parent.micro),Bitvector_Utils::NumberEncoding::BINARY), 0},index);
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
        uint32_t miniFIDIndex = Bitvector_Utils::decodeNumber(miniTreeParent.miniTopFIDIndex, Bitvector_Utils::NumberEncoding::BINARY) - 1;
        uint32_t firstLowMini = Bitvector_Utils::decodeNumber(miniFIDLowTree.at(miniFIDIndex), Bitvector_Utils::NumberEncoding::BINARY) - 1;
        uint32_t firstTopMini = Bitvector_Utils::decodeNumber(miniFIDTopTree.at(miniFIDIndex), Bitvector_Utils::NumberEncoding::BINARY) - 1;
        succinct_bv::BitVector miniTVSupport = miniTypeVectorsSupport.at(miniFIDIndex);
        succinct_bv::BitVector miniFIDSupport = miniFIDsSupport.at(miniFIDIndex);
        uint32_t fidRank = static_cast<uint32_t>(miniFIDSupport.Rank(index));
        uint32_t fidRankSelect = 0;
        if (fidRank != 0) {
            fidRankSelect = static_cast<uint32_t>(miniFIDSupport.Select(fidRank - 1));
        }
        uint32_t tvRank = static_cast<uint32_t>(miniTVSupport.Rank(fidRank - 1));
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
                miniRes = firstLowMini + (fidRank - 1) - tvRank;
            }
        }
        miniTreeParent = getMiniTree(miniRes);
    }

    if(checkMicro) {
        uint32_t microFIDIndex = Bitvector_Utils::decodeNumber(miniTreeParent.microTopFIDIndices.at(parent.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        uint32_t firstLowMicro = Bitvector_Utils::decodeNumber(miniTreeParent.microFIDLowTrees.at(microFIDIndex),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        uint32_t firstTopMicro = Bitvector_Utils::decodeNumber(miniTreeParent.microFIDTopTrees.at(microFIDIndex),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        succinct_bv::BitVector microTVSupport = miniTreeParent.typeVectorsSupport.at(microFIDIndex);
        succinct_bv::BitVector microFIDSupport = miniTreeParent.FIDsSupport.at(microFIDIndex);
        uint32_t fidRank = static_cast<uint32_t>(microFIDSupport.Rank(microIndexHelp));
        uint32_t fidRankSelect = 0;
        if(fidRank != 0) {
            fidRankSelect = static_cast<uint32_t>(microFIDSupport.Select(fidRank - 1));
        }
        uint32_t tvRank = static_cast<uint32_t>(microTVSupport.Rank(fidRank - 1));
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
                microRes = firstLowMicro + (fidRank - 1) - tvRank;
            }
        }
    }

    if(checkNode) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(getMiniTree(miniRes),microRes));
        uint32_t matSize = static_cast<uint32_t>(sqrt(entry.childMatrix.size()));
        uint32_t startRank = static_cast<uint32_t>(entry.childMatrixSupport.Rank(matSize * parent.node));
        uint32_t startRankSelect = static_cast<uint32_t>(entry.childMatrixSupport.Select(startRank + nodeIndexHelp));
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
    if(Bitvector_Utils::decodeNumber(miniDummys.at(miniRes),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeRes.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == microRes && Bitvector_Utils::decodeNumber(miniTreeRes.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == nodeRes ) {
            return {Bitvector_Utils::decodeNumber(miniTreeRes.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY), 0, 0};
        }
    }
    if(nodeRes != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeRes.dummys.at(microRes),Bitvector_Utils::NumberEncoding::BINARY) == nodeRes) {
            return {miniRes, Bitvector_Utils::decodeNumber(miniTreeRes.microDummyPointers.at(microRes),Bitvector_Utils::NumberEncoding::BINARY), 0};
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
    if(Bitvector_Utils::decodeNumber(miniDummys.at(parent.mini),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == parent.micro && Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == parent.node ) {
            return childRank(parent);
        }
    }
    if(parent.node != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.dummys.at(parent.micro),Bitvector_Utils::NumberEncoding::BINARY) == parent.node) {
            return childRank(parent);
        }
    }

    if(node.node > 0) {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTreeParent, parent.micro));
            uint32_t matSize = static_cast<uint32_t>(sqrt(entry.childMatrix.size()));
            uint32_t startRank = static_cast<uint32_t>(entry.childMatrixSupport.Rank(matSize * parent.node));
            uint32_t fullRank = static_cast<uint32_t>(entry.childMatrixSupport.Rank(matSize * parent.node + node.node));

        if(parent.node > 0) {
            return fullRank - startRank - 1;
        } else {
            res += fullRank - startRank - 1;
        }
    }

    if(node.micro > 0) {
        if (node.node!=0) {
            return res + Bitvector_Utils::decodeNumber(miniTree.microExtendedChildRanks.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        } else {
            res += Bitvector_Utils::decodeNumber(miniTree.microChildRanks.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }

    }

    if(parent.mini == node.mini) {
        uint32_t microFIDIndex = Bitvector_Utils::decodeNumber(miniTreeParent.microTopFIDIndices.at(parent.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        uint32_t firstMicro = Bitvector_Utils::decodeNumber(miniTreeParent.microFIDTopTrees.at(microFIDIndex),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        if (miniTreeParent.typeVectorsSupport.at(microFIDIndex).Rank(0) == 0) {
            firstMicro = Bitvector_Utils::decodeNumber(miniTreeParent.microFIDLowTrees.at(microFIDIndex),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
        uint32_t miniFIDIndex =Bitvector_Utils::decodeNumber(miniTreeParent.miniTopFIDIndex, Bitvector_Utils::NumberEncoding::BINARY) -1;
        uint32_t firstMini = Bitvector_Utils::decodeNumber(miniFIDTopTree.at(miniFIDIndex), Bitvector_Utils::NumberEncoding::BINARY) - 1;
        if (miniTypeVectorsSupport.at(miniFIDIndex).Rank(0) == 0) {
            firstMini = Bitvector_Utils::decodeNumber(miniFIDLowTree.at(miniFIDIndex), Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }

        if(node.mini != firstMini) {
            return res + Bitvector_Utils::decodeNumber(miniTree.microChildRanks.at(0),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
        else if(node.micro==0 && node.micro != firstMicro) {
            res += Bitvector_Utils::decodeNumber(miniTree.microExtendedChildRanks.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }
    } else {

        res += Bitvector_Utils::decodeNumber(miniTree.miniChildRank, Bitvector_Utils::NumberEncoding::BINARY);
    }
    return res;
}

HstNode HypersuccinctTree::getParentForQuery(HstNode node) {
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        Bitvector parentBit = entry.parentPointers.at(node.node);
        uint32_t parent = Bitvector_Utils::decodeNumber(parentBit,Bitvector_Utils::NumberEncoding::BINARY);
        return {node.mini,node.micro,parent - 1};
    }
    if(node.micro > 0) {
        Bitvector parentBit = miniTree.microParents.at(node.micro);
        uint32_t parentMicro = Bitvector_Utils::decodeNumber(parentBit,Bitvector_Utils::NumberEncoding::BINARY) - 1;
        uint32_t parentDummy = Bitvector_Utils::decodeNumber(miniTree.dummys.at(parentMicro),Bitvector_Utils::NumberEncoding::BINARY);
        uint32_t parentFID = Bitvector_Utils::decodeNumber(miniTree.microLowFIDIndices.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY);
        if(parentFID == 0) {
            return {node.mini,parentMicro,parentDummy};
        }
        return {node.mini,parentMicro,0};
    }
    uint32_t parentMini = Bitvector_Utils::decodeNumber(miniTree.miniParent,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    MiniTree parentMiniTree = getMiniTree(parentMini);
    uint32_t parentFID = Bitvector_Utils::decodeNumber(miniTree.miniLowFIDIndex,Bitvector_Utils::NumberEncoding::BINARY);
    if(parentFID == 0) {
        return {parentMini,Bitvector_Utils::decodeNumber(parentMiniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY),Bitvector_Utils::decodeNumber(parentMiniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY)};
    }
    return {parentMini,0,0};
}

HstNode HypersuccinctTree::getParent(HstNode node) {
    MiniTree miniTree = getMiniTree(node.mini);
    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
        Bitvector parentBit = entry.parentPointers.at(node.node);
        uint32_t parent = Bitvector_Utils::decodeNumber(parentBit,Bitvector_Utils::NumberEncoding::BINARY) - 1;
        return {node.mini,node.micro,parent};
    }
    if(node.micro > 0) {
        Bitvector parentBit = miniTree.microParents.at(node.micro);
        uint32_t parentMicro = Bitvector_Utils::decodeNumber(parentBit,Bitvector_Utils::NumberEncoding::BINARY) - 1;
        uint32_t parentDummy = Bitvector_Utils::decodeNumber(miniTree.dummys.at(parentMicro),Bitvector_Utils::NumberEncoding::BINARY);
        uint32_t parentFID = Bitvector_Utils::decodeNumber(miniTree.microLowFIDIndices.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY);
        if(parentFID == 0) {
            return getParent({node.mini,parentMicro,parentDummy});
        }
        return {node.mini,parentMicro,0};
    }
    uint32_t parentMini = Bitvector_Utils::decodeNumber(miniTree.miniParent,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    MiniTree parentMiniTree = getMiniTree(parentMini);
    uint32_t parentFID = Bitvector_Utils::decodeNumber(miniTree.miniLowFIDIndex,Bitvector_Utils::NumberEncoding::BINARY);
    if(parentFID == 0) {
        return getParent({parentMini,Bitvector_Utils::decodeNumber(parentMiniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY),Bitvector_Utils::decodeNumber(parentMiniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY)});
    }
    return {parentMini,0,0};
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
        Bitvector degreeB = entry.degree.at(node.node);
        return Bitvector_Utils::decodeNumber(degreeB,Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    if(node.micro > 0) {
        MiniTree miniTree = getMiniTree(node.mini);
        uint32_t fidIndex = Bitvector_Utils::decodeNumber(miniTree.microTopFIDIndices.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        return static_cast<uint32_t>(miniTree.FIDs.at(fidIndex).size());
    }
    uint32_t fidID = Bitvector_Utils::decodeNumber(getMiniTree(node.mini).miniTopFIDIndex,Bitvector_Utils::NumberEncoding::BINARY)-1;
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
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(subTreeB,Bitvector_Utils::NumberEncoding::BINARY);
            Bitvector subtreeSize = miniTree.microSubTrees.at(dummyPointer);
            res += Bitvector_Utils::decodeNumber(subtreeSize, Bitvector_Utils::NumberEncoding::BINARY);
        }
        if(isDummyAncestorWithinMiniTree(node)) {
            isDummyAncestor++;
            uint32_t dummyPointer = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            res += Bitvector_Utils::decodeNumber(getMiniTree(dummyPointer).subTree, Bitvector_Utils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;

        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
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
            Bitvector entryDepth = entry.nodeDepths.at(node.node);
            res -= (Bitvector_Utils::decodeNumber(entryDepth, Bitvector_Utils::NumberEncoding::BINARY) - 1);
        }
        else {
            LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTree,node.micro));
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
        Bitvector entryLeafSize = entry.leaves.at(node.node);
        return res + Bitvector_Utils::decodeNumber(entryLeafSize, Bitvector_Utils::NumberEncoding::BINARY);
    }
    if(node.micro > 0) {
        uint32_t isDummyAncestor = 0;
        if(isDummyAncestorWithinMiniTree({node.mini,node.micro,0})) {
            isDummyAncestor = 1;
            uint32_t newMini = Bitvector_Utils::decodeNumber(miniTree.miniDummyPointer,Bitvector_Utils::NumberEncoding::BINARY);
            MiniTree newMiniTree = getMiniTree(newMini);
            res += Bitvector_Utils::decodeNumber(newMiniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
        }
        res -= isDummyAncestor;
        Bitvector microLeafSize = miniTree.microLeaves.at(node.micro);
        return res + Bitvector_Utils::decodeNumber(microLeafSize, Bitvector_Utils::NumberEncoding::BINARY);
    }
    return res + Bitvector_Utils::decodeNumber(miniTree.miniLeaves, Bitvector_Utils::NumberEncoding::BINARY);
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
    if(Bitvector_Utils::decodeNumber(miniDummys.at(parent.mini),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == parent.micro && Bitvector_Utils::decodeNumber(miniTreeParent.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == parent.node ) {
            return leafRank(parent);
        }
    }
    if(parent.node != 0) {
        if(Bitvector_Utils::decodeNumber(miniTreeParent.dummys.at(parent.micro),Bitvector_Utils::NumberEncoding::BINARY) == parent.node) {
            return leafRank(parent);
        }
    }

    if(node.node > 0) {
        LookupTableEntry entry = getLookupTableEntry(getMicroTree(miniTreeParent, parent.micro));
        if(Bitvector_Utils::decodeNumber(miniDummys.at(node.mini),Bitvector_Utils::NumberEncoding::BINARY) != 0) {
            if(Bitvector_Utils::decodeNumber(miniTree.miniDummyTree,Bitvector_Utils::NumberEncoding::BINARY) == node.micro && Bitvector_Utils::decodeNumber(miniTree.miniDummyIndex,Bitvector_Utils::NumberEncoding::BINARY) == node.node ) {
                res--;
            }
        }
        if(node.node != 0) {
            if(Bitvector_Utils::decodeNumber(miniTree.dummys.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) == node.node) {
                res--;
            }
        }
        res += Bitvector_Utils::decodeNumber(entry.leafRank.at(node.node),Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }

    if(node.micro > 0) {
        if (node.node!=0) {
            res += Bitvector_Utils::decodeNumber(miniTree.microExtendedLeafRanks.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        } else {
            res += Bitvector_Utils::decodeNumber(miniTree.microRootLeafRanks.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
        }

    }
    if(parent.mini != node.mini) {
        return res + Bitvector_Utils::decodeNumber(miniTree.miniRootLeafRank, Bitvector_Utils::NumberEncoding::BINARY);
    }
    uint32_t microFIDIndex = Bitvector_Utils::decodeNumber(miniTreeParent.microTopFIDIndices.at(parent.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
    uint32_t firstMicro = Bitvector_Utils::decodeNumber(miniTreeParent.microFIDTopTrees.at(microFIDIndex),Bitvector_Utils::NumberEncoding::BINARY) - 1;
    if (miniTreeParent.typeVectorsSupport.at(microFIDIndex).Rank(0) == 0) {
        firstMicro = Bitvector_Utils::decodeNumber(miniTreeParent.microFIDLowTrees.at(microFIDIndex),Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    uint32_t miniFIDIndex =Bitvector_Utils::decodeNumber(miniTreeParent.miniTopFIDIndex, Bitvector_Utils::NumberEncoding::BINARY) -1;
    uint32_t firstMini = Bitvector_Utils::decodeNumber(miniFIDTopTree.at(miniFIDIndex), Bitvector_Utils::NumberEncoding::BINARY) - 1;
    if (miniTypeVectorsSupport.at(miniFIDIndex).Rank(0) == 0) {
        firstMini = Bitvector_Utils::decodeNumber(miniFIDLowTree.at(miniFIDIndex), Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }

    if(node.mini != firstMini) {
        return res + Bitvector_Utils::decodeNumber(miniTree.microRootLeafRanks.at(0),Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    else if(node.micro==0 && node.micro != firstMicro) {
        res += Bitvector_Utils::decodeNumber(miniTree.microExtendedLeafRanks.at(node.micro),Bitvector_Utils::NumberEncoding::BINARY) - 1;
    }
    res += Bitvector_Utils::decodeNumber(miniTree.miniRootLeafRank, Bitvector_Utils::NumberEncoding::BINARY);

    return res;
}
