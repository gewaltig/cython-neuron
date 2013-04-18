import sys
import re
from os.path import expanduser
from os.path import join
from os import listdir
from ctypes import *

# begin of class wrappers

cdef class DataConverter:
    cdef classes.DatumToPythonConverter *dTp
    cdef classes.NESTEngine *pTd

    def __cinit__(self):
        self.dTp = new classes.DatumToPythonConverter()
        self.pTd = new classes.NESTEngine()

    def __dealloc__(self):
        del self.dTp
        del self.pTd

    cdef object datumToObject(self, classes.Datum* d):
        return self.dTp.convertDatum(d)

    cdef classes.Datum* objectToDatum(self, object o):
        return self.pTd.PyObject_as_Datum(o)

    cdef void updateDictionary(self, classes.Datum* src, classes.Datum* dest):
        self.dTp.updateDictionary(src, dest)


# this class represents the entry point with which the cython_neuron.cpp class can access to the cython side
cdef class CythonEntry:
    cdef classes.CythonEntry *thisptr
    def __cinit__(self):
        self.thisptr= new classes.CythonEntry()
        
    def __dealloc__(self):
        del self.thisptr

    cdef putEntry(self, void* value):
        self.thisptr.putEntry(value)
    
    cdef void* getEntry(self):
        return self.thisptr.getEntry()

# end of class wrappers


cdef DataConverter converter = DataConverter()
loadedNeurons = {}
modelsFolder = expanduser("~") + "/Programs/Nest/cython_models"


def returnNeuronName(cmd):
    m = re.search('^{ /(.+?) .*Create }.*$', cmd)
    if hasattr(m, 'group'):
        return m.group(1)
    else:
        return ""

def getDynamicNeuronsName():
    listSo = listdir(modelsFolder)
    return [so[0:len(so) - 3] for so in listSo if ".so" in so]

# this method is called at every execution of the cynest.Create() method and seeks for dynamic neurons. If the neuron is dynamic,
# it is loaded for further utilization
def processNeuronCreation(cmd):
    n = returnNeuronName(cmd)
    if n is not "":
        nList = getDynamicNeuronsName()
        if n in nList and not loadedNeurons.has_key(n):
             libc = PyDLL(modelsFolder + "/" + n + ".so")
             exec("libc.init" + n + "()")
             loadedNeurons[n] = libc


# this method updates the neuron members based on the parameters argument
cdef void setNeuronMembers(bytes neuronName, int neuronID, classes.Datum* parameters) with gil:
        cdef dict members = <dict>converter.datumToObject(parameters)
        po = py_object(members)
        loadedNeurons[neuronName].setNeuronParams.argtypes = [c_int, py_object]

        loadedNeurons[neuronName].setNeuronParams(neuronID, po)


# this method retrieves the neuron members and puts them in the parameters argument
cdef void retrieveNeuronMembers(bytes neuronName, int neuronID, classes.Datum* parameters) with gil:
    cdef string key
    loadedNeurons[neuronName].getNeuronParams.restype = py_object

    value = loadedNeurons[neuronName].getNeuronParams(neuronID)
    cdef classes.Datum* members = converter.objectToDatum(value)
    converter.updateDictionary(members, parameters)



# this is the only callable method from cython_neuron.cpp. One can pass the command to execute and the parameters dictionary, which will be updated
cdef int cEntry(string neuronName, int neuronID, string cmd, classes.Datum* args) with gil:
        cdef bytes cmdBytes = cmd.encode('UTF-8')
        cdef bytes nNBytes = neuronName.encode('UTF-8')
        
        if args is NULL:
              return -1


        # special initialization command
        if cmdBytes == "_{init}_":
              nID =  <int>loadedNeurons[nNBytes].createNeuron()
              setNeuronMembers(nNBytes, nID, args)
              return nID

        else:
              setNeuronMembers(nNBytes, neuronID, args)
              exec("loadedNeurons[nNBytes]."+ cmdBytes + "(neuronID)")
              retrieveNeuronMembers(nNBytes, neuronID, args)
              return neuronID

