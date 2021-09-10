#distutils: language=c++

from cython.operator cimport dereference
from libcpp.memory cimport unique_ptr

from pht_xml_reader cimport XMLReader
from pht_unordered_tree cimport UnorderedTree
from pht_hst_factory cimport HypersuccinctTreeFactory
from pht_hst cimport HypersuccinctTree

from pht_hst_node cimport HstNode

cdef class PyHST:
    cdef unique_ptr[HypersuccinctTree] hst

    def __cinit__(self, path, huffman=False, sizeMini=0, sizeMicro=0):
        self.hst = HypersuccinctTreeFactory.create(XMLReader.read(path.encode('UTF-8')), huffman, sizeMini, sizeMicro)

    def degree(self, mini, micro, node):
        return dereference(self.hst).degree(HstNode(mini, micro, node))