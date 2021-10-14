from setuptools import setup
from distutils.core import Extension
from Cython.Build import cythonize

setup(ext_modules = cythonize(
        [Extension("pht_hst", 
            sources=["pht_hst.pyx", "../src/bitvector_utils.cpp", "../src/hst_node.cpp", "../src/hst_output.cpp", "../src/hypersuccinct_tree.cpp", "../src/hypersuccinct_tree_queries.cpp", "../src/logger.cpp", "../src/timer.cpp", "../src/xml_reader.cpp", "../extern/irrxml/irrXML.cpp", "../extern/succinct_bv/src/bit_vector.cc", ],
            extra_compile_args=["/std:c++17", ]
        )],
        build_dir="build"
    ), 
    include_dirs=["../include", "../extern/irrxml", "../extern/succinct_bv/include", ]
)
