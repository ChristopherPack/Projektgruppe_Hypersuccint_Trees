from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.memory cimport unique_ptr
from libcpp.string cimport string

from pht_unordered_tree cimport UnorderedTree
from pht_hst cimport HypersuccinctTree
    
cdef extern from "../include/pht/hypersuccinct_tree_factory.h" namespace "pht":
    cdef cppclass HypersuccinctTreeFactory:
        @staticmethod
        unique_ptr[HypersuccinctTree] create(shared_ptr[UnorderedTree[string]] tree, bool huffman, unsigned int sizeMini, unsigned int sizeMicro)