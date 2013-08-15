# Cython wrapper for the pynest kernel functions.
# This is a cython 'header' file that is read by other modules.
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "psignal.h":
  cdef int SLIsignalflag
  cdef void SLISignalHandler(int)
  
cdef extern from "datum.h":
  cdef cppclass Datum:
        pass


cdef extern from "token.h":
    cdef cppclass Token:
        Token(Token)
        Token(Datum *)

cdef extern from "cynestkernel.h":
    cdef cppclass NESTEngine:
        NESTEngine()
        bint init(vector[string] argv, string)
        bint push(object)
        bint push_token(Token)
        bint push_connections(object)
        bint run(string)
        bint run_token(Token)
        object pop()
        Token* pop_token()
        Datum* PyObject_as_Datum(object)
        bint check_engine()
        void register_cython_model(string)

cdef extern from "time_scheduler.h":
    cdef cppclass TimeScheduler:
        TimeScheduler()
        double get_ms(int, long, double)
        long get_tics_or_steps(int, int, long, double)
        unsigned int get_scheduler_value(int, unsigned int)
        

