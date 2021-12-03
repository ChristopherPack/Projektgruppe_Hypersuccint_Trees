from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from pht_unordered_tree cimport UnorderedTree
    
cdef extern from "../include/pht/xml_reader.h" namespace "pht":
    cdef cppclass XMLReader:
        @staticmethod
        shared_ptr[UnorderedTree[string]] read(string path);