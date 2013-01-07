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



        
