#distutils: language=c++

from libcpp.string cimport string

from pht_timer cimport Timer

cdef class PyTimer:
    cdef Timer tmr

    def __cinit__(self):
        self.tmr = Timer()

    def start(self):
        self.tmr.start()

    def stop(self):
        self.tmr.stop()

    def toString(self):
        return self.tmr.toString().decode('UTF-8')