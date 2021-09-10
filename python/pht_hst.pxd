from pht_hst_node cimport HstNode
from libcpp.memory cimport unique_ptr

cdef extern from "../include/pht/hypersuccinct_tree.h" namespace "pht":
    cdef cppclass HypersuccinctTree:
        bint isDummyAncestorWithinMiniTree(HstNode) except +
        bint isDummyAncestorWithinMicroTree(HstNode) except +
        HstNode child(HstNode, unsigned int) except +
        unsigned int childRank(HstNode) except +
        HstNode getParent(HstNode) except +
        unsigned int degree(HstNode) except +
        unsigned int subtreeSize(HstNode) except +
        unsigned int depth(HstNode) except +
        unsigned int height(HstNode) except +
        HstNode leftmostLeaf(HstNode) except +
        HstNode rightmostLeaf(HstNode) except +
        unsigned int leafSize(HstNode) except +
        unsigned int leafRank(HstNode) except +
#        HstNode levelAncestor(HstNode, level) except +