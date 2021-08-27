from libcpp.string cimport string

cdef extern from "../src/timer.cpp":
    pass
    
cdef extern from "../include/pht/timer.h" namespace "pht":
    cdef cppclass Timer:
        Timer() except +
        void start()
        void stop()
        string toString()