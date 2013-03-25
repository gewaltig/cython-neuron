import sys
import re
from os.path import expanduser
from os.path import join
from os import listdir
from ctypes import *

# begin of class wrappers

cdef class Cy_Dict:
    cdef classes.Cy_Dict *thisptr

    def __cinit__(self):
        pass

    def __dealloc__(self):
        del self.thisptr

    cdef void init(self, classes.Cy_Dict* ptr):
        self.thisptr = ptr

    cdef void setObject(self, string key, double value):
            self.thisptr.setObject(key, value)

    cdef void removeObject(self, string key):
            self.thisptr.removeObject(key)

    cdef double getObject(self, string key):
            return self.thisptr.getObject(key)

    cdef void clear(self):
            self.thisptr.clear()

    cdef void resetIterator(self):
            self.thisptr.resetIterator()

    cdef void nextElement(self):
            self.thisptr.nextElement()

    cdef string getCurrentKey(self):
            return self.thisptr.getCurrentKey()

    cdef double getCurrentValue(self):
            return self.thisptr.getCurrentValue()

    cdef int hasElement(self, string key):
            return self.thisptr.hasElement(key)

    cdef int getLength(self):
            return self.thisptr.getLength()



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


cdef Cy_Dict params = Cy_Dict()
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
# it is loaded for funrther utilization
def processNeuronCreation(cmd):
    n = returnNeuronName(cmd)
    if n is not "":
        nList = getDynamicNeuronsName()
        if n in nList and not loadedNeurons.has_key(n):
             libc = PyDLL(modelsFolder + "/" + n + ".so")
             exec("libc.init" + n + "()")
             loadedNeurons[n] = libc


# this method updates the neuron members based on the parameters argument
cdef void setNeuronMembers(bytes neuronName, int neuronID, Cy_Dict parameters) with gil:
    parameters.resetIterator()

    for num in range(0, parameters.getLength()):
        loadedNeurons[neuronName].setNeuronParam(neuronID, parameters.getCurrentKey().encode('UTF-8'), c_double(parameters.getCurrentValue()))
        parameters.nextElement()




cdef string transformIntoString(source):
    cdef int i = 0
    cdef string output = ""

    while source[i] is not '\0':
        output = output + source[i]
        i = i + 1
    return output

# this method retrieves the neuron members and puts them in the parameters argument
cdef void retrieveNeuronMembers(bytes neuronName, int neuronID, Cy_Dict parameters) with gil:
    cdef string key
    loadedNeurons[neuronName].getNeuronParam.restype = c_double
    loadedNeurons[neuronName].initIteratorNeuronParams(neuronID)

    if loadedNeurons[neuronName].hasNeuronNextParam(neuronID) == 1:
        keyC = create_string_buffer('\000' * 100)
        value = loadedNeurons[neuronName].getNeuronParam(neuronID, keyC)
        key = transformIntoString(keyC)

        if not key.startswith("_"): # it's a public field
              parameters.setObject(key, value)


# this is the only callable method from cython_neuron.cpp. One can pass the command to execute and the parameters dictionary, which will be updated
cdef int cEntry(string neuronName, int neuronID, string cmd, classes.Cy_Dict* args) with gil:
        cdef bytes cmdBytes = cmd.encode('UTF-8')
        cdef bytes nNBytes = neuronName.encode('UTF-8')
        
        if args is NULL:
              return -1

        params.init(args)

        # special initialization command
        if cmdBytes == "_{init}_":
              nID =  <int>loadedNeurons[nNBytes].createNeuron()
              setNeuronMembers(nNBytes, nID, params)
              return nID

        else:
              setNeuronMembers(nNBytes, neuronID, params)
              exec("loadedNeurons[nNBytes]."+ cmdBytes + "(neuronID)")
              retrieveNeuronMembers(nNBytes, neuronID, params)
              return neuronID

