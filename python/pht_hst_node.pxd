from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

cdef extern from "../include/pht/hst_node.h" namespace "pht":
    cdef cppclass HstNode:
        unsigned int mini
        unsigned int micro
        unsigned int node
        HstNode()
        HstNode(unsigned int, unsigned int, unsigned int) except +