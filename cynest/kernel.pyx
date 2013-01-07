# Cython wrapper for the pynest functions.
from libcpp.string cimport string
from libcpp.vector cimport vector
cimport cpython # we need this for the unicode UTF-8 conversion

# This imports the C++ class wrappers
cimport classes

cdef class NESTEngine:
    cdef classes.NESTEngine *thisptr
    def __cinit__(self):
        self.thisptr= new classes.NESTEngine()
    def __dealloc__(self):
        print ("CyNEST says good bye.")
        del self.thisptr
        
    def init(self, argv, modulepath):
        """
        Startup the NEST engine.
        argv: list of strings with NEST's command line arguments
        modulepath: the path to CyNEST's startup file
        The function will return true if startup succeded and
        false otherwise.
        This function is part of the low-level API.
        """
        if len(argv[0]) == 0:
            argv[0]='cynest'
        argv_bytes= [ str.encode('UTF-8') for str in argv]
        cdef bytes modulepath_bytes=modulepath.encode('UTF-8')
        return self.thisptr.init(argv_bytes, modulepath_bytes)

    def push(self, value):
        """
        Push a Python object onto NEST's operand stack.
        value can be almost any Python object.
        Not pushable are dictionaries with non-string keys.
        This function is part of the low-level API.
        """
        return self.thisptr.push(value)

    def pop(self):
        """
        Pop the top value from NEST's operand stack and return it as
        Python object.
        If the stack is empty, a NESTError exception is raised.
        This function is part of the low-level API.
        """
        return self.thisptr.pop()

    def run(self, command):
        """
        Execute a SLI command string.
        SLI is the native language of NEST. This function takes a string which is then parsed and executed by
        NEST. The string may contain an arbitrary sequence of NEST commands.
        If errors occur during execution, they are reported back to Python as NESTError exections.
        This function is part of the low-level API.
        """
        cdef bytes command_bytes=command.encode('UTF-8')
        return self.thisptr.run(command_bytes)

    cpdef run_pytoken(self, PyToken command):
        """
        Execute a PyDatum object.
        This function is part of the low-level API.
        """
        return self.thisptr.run_token(command.thisptr[0])

    def pop_pytoken(self):
         t= PyToken()
         t.thisptr= self.thisptr.pop_token()
         return t

    def push_connections(self, connectome):
        """
        Push a list of dictionaries with connection information to NEST. Each dictionary is converted to a NEST ConnectionDatum
        which encapsulates the five-tuple defining a connecion.
        This function is part of the low-level API.
        """
        return self.thisptr.push_connections(connectome)
    
    def check_engine(self):
        """
        Returns true if the object is properly initialized and false otherwise.
        """
        return self.thisptr.check_engine()

cdef public class PyToken[object PyToken, type PyTokenType]:
     """
     Python wrapper of SLI's Token class.
     """
     cdef classes.Token *thisptr
     def __dealloc__(self):
         if self.thisptr:
            del self.thisptr

cdef class NameDatum:
     """
     Python wrapper of SLI's NameDatum class. 
     This class is used to store SLI commands and variables.
     """
     cdef classes.NameDatum *thisptr
     def __cinit__(self, name):
         cdef string name_b=name.encode('UTF-8')
         self.thisptr = new classes.NameDatum(name_b)

     def __dealloc__(self):
         del self.thisptr

     def as_PyToken(self):
         """
         Create a new Token with the NameDatum.
         """
         t=PyToken()
         cdef classes.NameDatum *name_ptr=new classes.NameDatum(self.thisptr[0])
         t.thisptr= new classes.Token(<classes.Datum *>name_ptr)
         return t

cdef public object Token_to_PyObject(classes.Token *arg):
     """
     Convert a Datum pointer to a Python object.
     """
     dat=PyToken()
     dat.thisptr=arg
     return dat

