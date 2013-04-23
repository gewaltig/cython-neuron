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

cdef extern from "namedatum.h":
    cdef cppclass NameDatum:
        NameDatum(string)
        NameDatum(NameDatum)
        string toString()

cdef extern from "token.h":
    cdef cppclass Token:
        Token(Token)
        Token(Datum *)

cdef extern from "datumtopythonconverter.h":
    cdef cppclass DatumToPythonConverter:
        DatumToPythonConverter()
        object convertDatum(Datum*)
        void updateDictionary(Datum* src, Datum* dest)

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

cdef extern from "buffer.h":
    cdef cppclass CythonEntry:
        CythonEntry()
        void putEntry(void* value)
        void* getEntry()

cdef extern from "nest_time.h":
    cdef cppclass Time:
        cppclass step:
            step(long t)
        cppclass tic:
            tic(long t)
        cppclass ms:
            ms(double t)
            ms(long t)
        cppclass ms_stamp:
            ms_stamp(double t)
            ms_stamp(long t)
        Time()
        Time(Time.step t)
        Time(Time.tic t)
        Time(Time.ms t)
        Time(Time.ms_stamp t)
        long get_tics()
        long get_steps()
        double get_ms()

cdef extern from "nest_time.h" namespace "nest::Time":
    Time get_resolution()

cdef extern from "scheduler.h" namespace "nest::Scheduler":
    unsigned int get_modulo(unsigned int d)
    unsigned int get_slice_modulo(unsigned int d)
    unsigned int get_min_delay()
    unsigned int get_max_delay()

