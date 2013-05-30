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
        void putInit(void* value)
        void* getInit()
        void putCalibrate(void* value)
        void* getCalibrate()
        void putUpdate(void* value)
        void* getUpdate()
        void putSetStatus(void* value)
        void* getSetStatus()
        void putGetStatus(void* value)
        void* getGetStatus()
        void putStdVars(void* value)
        void* getStdVars()
        void putDestroy(void* value)
        void* getDestroy()

    cdef cppclass SpecialFunctions:
        SpecialFunctions()
        double get_ms(int, long, double)
        long get_tics_or_steps(int, int, long, double)
        unsigned int get_scheduler_value(int, unsigned int)

        

