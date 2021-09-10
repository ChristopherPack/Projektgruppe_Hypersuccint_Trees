from pht_hst_node cimport HstNode

cdef extern from "../include/pht/hypersuccinct_tree.h" namespace "pht":
    cdef cppclass HypersuccinctTree:
        unsigned int degree(HstNode) except +