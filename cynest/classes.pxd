# Cython wrapper for the pynest kernel functions.
# This is a cython 'header' file that is read by other modules.
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "cynestkernel.h":
    cdef cppclass NESTEngine:
        NESTEngine()
        bint init(vector[string] argv, string)
        bint push(object)
        bint push_connections(object)
        bint run(string)
        object pop()
        bint check_engine()

cdef extern from "datum.h":
    cdef cppclass Datum:
        pass

cdef extern from "namedatum.h":
    cdef cppclass NameDatum:
        NameDatum(string)
        string toString()
        
