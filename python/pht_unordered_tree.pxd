from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

cdef extern from "../include/pht/unordered_tree.h" namespace "pht":
    cdef cppclass UnorderedTree[T]:
        pass