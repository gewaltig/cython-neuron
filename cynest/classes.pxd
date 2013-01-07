# Cython wrapper for the pynest kernel functions.
# This is a cython 'header' file that is read by other modules.
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "datum.h":
  cdef cppclass Datum:
        pass

cdef extern from "namedatum.h":
    cdef cppclass NameDatum:
        NameDatum(string)
        NameDatum(NameDatum)
        string toString()

cdef extern from "token.h":
    cdef cppclass Token:
        Token(Token)
        Token(Datum *)

cdef extern from "cynestkernel.h":
    cdef cppclass NESTEngine:
        NESTEngine()
        bint init(vector[string] argv, string)
        bint push(object)
        bint push_connections(object)
        bint run(string)
        bint run_token(Token)
        object pop()
        Token* pop_token()
        bint check_engine()


