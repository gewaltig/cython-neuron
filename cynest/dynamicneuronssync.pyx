import sys
import re
import os
import errno
from os import listdir
from ctypes import *

# begin of class wrappers

# This class is useful for converting Datum into python objects and vice-versa
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

# This class contains the special functions needed by
# the cython_neuron in order to access the Time and Scheduler classes
# Note that other methods having nothing to do with that are present.
# modelsFolder has been put into that class otherwise it's not persistent 
# during the execution.
cdef class SpecialFunctions:
    cdef classes.SpecialFunctions *thisptr
    cdef string modelsFolder

    def __cinit__(self):
        self.thisptr= new classes.SpecialFunctions()
        
    def __dealloc__(self):
        del self.thisptr

    cdef double get_ms(self, int arg1, long arg2, double arg3):
        return self.thisptr.get_ms(arg1, arg2, arg3)

    cdef long get_tics_or_steps(self, int arg1, int arg2, long arg3, double arg4):
        return self.thisptr.get_tics_or_steps(arg1, arg2, arg3, arg4)

    cdef unsigned int get_scheduler_value(self, int arg1, unsigned int arg2):
        return self.thisptr.get_scheduler_value(arg1, arg2)

    cdef void setModelsFolder(self, string value):
        self.modelsFolder = value

    cdef string getModelsFolder(self):
        return self.modelsFolder

# Class for storing the pointer to Standard Parameters
cdef class StandardParams:
    cdef long* spike
    cdef double* in_spikes
    cdef double* ex_spikes
    cdef double* currents
    cdef long* lag

    def __cinit__(self):
        pass
        
    def __dealloc__(self):
        pass


    cdef void setStdVars(self, long* spikeB, double* in_spikesD, double* ex_spikesD, double* currentsD, long* lagI):
        self.spike = spikeB
        self.in_spikes = in_spikesD
        self.ex_spikes = ex_spikesD
        self.currents = currentsD
        self.lag = lagI




# this class represents the entry point with which the cython_neuron.cpp class can access to the cython side
cdef class CythonEntry:
    cdef classes.CythonEntry *thisptr
    def __cinit__(self):
        self.thisptr= new classes.CythonEntry()
        
    def __dealloc__(self):
        del self.thisptr

    cdef void putInit(self, void* value):
        self.thisptr.putInit(value)
    
    cdef void* getInit(self):
        return self.thisptr.getInit()

    cdef void putCalibrate(self, void* value):
        self.thisptr.putCalibrate(value)
    
    cdef void* getCalibrate(self):
        return self.thisptr.getCalibrate()

    cdef void putUpdate(self, void* value):
        self.thisptr.putUpdate(value)
    
    cdef void* getUpdate(self):
        return self.thisptr.getUpdate()

    cdef void putSetStatus(self, void* value):
        self.thisptr.putSetStatus(value)
    
    cdef void* getSetStatus(self):
        return self.thisptr.getSetStatus()

    cdef void putGetStatus(self, void* value):
        self.thisptr.putGetStatus(value)
    
    cdef void* getGetStatus(self):
        return self.thisptr.getGetStatus()

    cdef void putStdVars(self, void* value):
        self.thisptr.putStdVars(value)
    
    cdef void* getStdVars(self):
        return self.thisptr.getStdVars()

    cdef void putDestroy(self, void* value):
        self.thisptr.putDestroy(value)
    
    cdef void* getDestroy(self):
        return self.thisptr.getDestroy()

# end of class wrappers


# Global objects
cdef DataConverter converter = DataConverter()
loadedNeurons = {}
cdef SpecialFunctions spFct = SpecialFunctions()
cdef stdParams = {}

# This method tries to find the Cython_models folder based on
# the location of the executable
cdef void setModelsFolder(bytes kernelDir):
    path1, path2 = os.path.split(kernelDir)
    path3, path4 = os.path.split(path1)
    path5, path6 = os.path.split(path3)
    path7, path8 = os.path.split(path5)
    
    spFct.setModelsFolder(path7 + os.sep + "cython_models")
    # we try to create the folder (even if it already present)
    try:
        os.makedirs(spFct.getModelsFolder())
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise



# Special Functions helpers (will point to the good C++ function)
def get_ms(arg1, arg2, arg3):
    return spFct.get_ms(arg1, arg2, arg3)

def get_tics_or_steps(arg1, arg2, arg3, arg4):
    return spFct.get_tics_or_steps(arg1, arg2, arg3, arg4)

def get_scheduler_value(arg1, arg2):
    return spFct.get_scheduler_value(arg1, arg2)


GETMSFUNC = CFUNCTYPE(c_double, c_int, c_long, c_double)
GETTICSORSTEPSFUNC = CFUNCTYPE(c_long, c_int, c_int, c_long, c_double)
GETSCHEDULERVALUE = CFUNCTYPE(c_uint, c_int, c_uint)
getmsFCT = GETMSFUNC(get_ms)
getticsorstepsFCT = GETTICSORSTEPSFUNC(get_tics_or_steps)
getschedulervalueFCT = GETSCHEDULERVALUE(get_scheduler_value)
# End of Special Function helpers


# Matches a string of the type ... Create is order to find 
# creations commands. It it's the case, returns the neuron name
def returnNeuronName(cmd):
    m = re.search('^{ /(.+?) .*Create }.*$', cmd)
    if hasattr(m, 'group'):
        return m.group(1)
    else:
        return ""

# Loads the list of the neuron names contained in the cython_models folder.
def getDynamicNeuronsName():
    listSo = listdir(spFct.getModelsFolder())
    return [so[0:len(so) - 3] for so in listSo if ".so" in so]


# Loads a new neuron type based on the name. It implies a file called
# <name>.so is present is the cython_models folder
def loadNewNeuron(n):
    if not loadedNeurons.has_key(n):
        try:
            libc = PyDLL(spFct.getModelsFolder() + "/" + n + ".so")
            exec("libc.init" + n + "()")
            loadedNeurons[n] = libc
            loadedNeurons[n].putSpecialFunctions.restype = None
            loadedNeurons[n].putSpecialFunctions(getmsFCT, getticsorstepsFCT, getschedulervalueFCT)
            stdParams[n] = [] # new neuron name creation for standard parameters
        except:
            pass


# this method is called at every execution of the cynest.Create() method and seeks for dynamic neurons. 
#If the neuron is dynamic, it is loaded for further utilization
def processNeuronCreation(cmd):
    n = returnNeuronName(cmd)
    if n is not "":
        nList = getDynamicNeuronsName()
        if n in nList:
            loadNewNeuron(n)


# this method updates the neuron members based on the parameters argument
cdef void setNeuronMembers(bytes neuronName, int neuronID, classes.Datum* parameters) with gil:
    cdef dict members = <dict>converter.datumToObject(parameters)
    loadedNeurons[neuronName].setNeuronParams.argtypes = [c_int, py_object]
    loadedNeurons[neuronName].setNeuronParams(neuronID, py_object(members))


# this method retrieves the neuron members and puts them in the parameters argument
cdef void retrieveNeuronMembers(bytes neuronName, int neuronID, classes.Datum* parameters) with gil:
    cdef string key
    loadedNeurons[neuronName].getNeuronParams.restype = py_object

    cdef classes.Datum* members = converter.objectToDatum(loadedNeurons[neuronName].getNeuronParams(neuronID))
    converter.updateDictionary(members, parameters)
    del members


# Pointers used to store the address of Standard Parameters passed to the cython neuron
sI = c_long()
isD = c_double()
esD = c_double()
cD = c_double()
lI = c_long()
sIR = byref(sI)
isDR = byref(isD)
esDR = byref(esD)
cDR = byref(cD)
lIR = byref(lI)

cdef void cUpdate(string nName, int neuronID) with gil:
    cdef bytes neuronName = nName.encode('UTF-8')

    if not loadedNeurons.has_key(neuronName):
        return

    cdef StandardParams sp = stdParams[neuronName][neuronID]
    loadedNeurons[neuronName].setStdVars(neuronID, sp.spike[0], sp.in_spikes[0], sp.ex_spikes[0], sp.currents[0], sp.lag[0])

    loadedNeurons[neuronName].update(neuronID)

    loadedNeurons[neuronName].getStdVars(neuronID, sIR, isDR, esDR, cDR, lIR)
    sp.spike[0] = sI.value
    sp.in_spikes[0] = isD.value
    sp.ex_spikes[0] = esD.value
    sp.currents[0] = cD.value
    sp.lag[0] = lI.value


# Initialization method. Called from the C++ side. Creates a new neuron and
# synchronizes the parameters (set and retrieve NeuronMembers)
# Returns the id of the new neuron
cdef int cInit(string neuronName, classes.Datum* args) with gil:
        cdef bytes nNBytes = neuronName.encode('UTF-8')

        loadNewNeuron(nNBytes)

        if not loadedNeurons.has_key(nNBytes):
            return -1

        # special initialization command
        nID =  <int>loadedNeurons[nNBytes].createNeuron()
        stdParams[nNBytes].append(StandardParams())
        loadedNeurons[nNBytes].getStdVars.argtypes = [c_int, c_void_p, c_void_p, c_void_p, c_void_p, c_void_p]
        loadedNeurons[nNBytes].setStdVars.argtypes = [c_int, c_long, c_double, c_double, c_double, c_long]
        setNeuronMembers(nNBytes, nID, args)

        retrieveNeuronMembers(nNBytes, nID, args)
        return nID

# Calibration method. Called from the C++ side. Calls the
# calibrate method of the cython neuron
cdef void cCalibrate(string neuronName, int neuronID, classes.Datum* args) with gil:
        cdef bytes nNBytes = neuronName.encode('UTF-8')

        if not loadedNeurons.has_key(nNBytes):
            return

        setNeuronMembers(nNBytes, neuronID, args)
        loadedNeurons[nNBytes].calibrate(neuronID)
        retrieveNeuronMembers(nNBytes, neuronID, args)
   
# SetStatus method. Called from the C++ side. Calls the
# setStatus method of the cython neuron     
cdef void cSetStatus(string neuronName, int neuronID, classes.Datum* args) with gil:
        cdef bytes nNBytes = neuronName.encode('UTF-8')
  
        if not loadedNeurons.has_key(nNBytes):
            return
      
        setNeuronMembers(nNBytes, neuronID, args)
        loadedNeurons[nNBytes].setStatus(neuronID)
        retrieveNeuronMembers(nNBytes, neuronID, args)

# GetStatus method. Called from the C++ side. Calls the
# getStatus method of the cython neuron
cdef void cGetStatus(string neuronName, int neuronID, classes.Datum* args) with gil:
        cdef bytes nNBytes = neuronName.encode('UTF-8')
  
        if not loadedNeurons.has_key(nNBytes):
            return
      
        loadedNeurons[nNBytes].getStatus(neuronID)
        retrieveNeuronMembers(nNBytes, neuronID, args)

# Standard Parameters method. Called from the C++ side. 
# Puts the S.P. pointers into a temporary structure
# used when calling the update function.
cdef void cStdVars(string neuronName, int neuronID, long* spike, double* in_spikes, double* ex_spikes, double* currents, long* lag) with gil:
    cdef bytes nNBytes = neuronName.encode('UTF-8')

    if not loadedNeurons.has_key(nNBytes):
        return

    cdef StandardParams sp = StandardParams()
    sp.setStdVars(spike, in_spikes, ex_spikes, currents, lag)
    stdParams[nNBytes][neuronID] = sp

# Destroying method. Called from the C++ side. Calls the
# destroyer of the neuron. Frees the library if no neuron
# of that type is present
cdef void cDestroy(string neuronName, int neuronID) with gil:
    cdef bytes nNBytes = neuronName.encode('UTF-8')
  
    if not loadedNeurons.has_key(nNBytes):
        return
      
    loadedNeurons[nNBytes].destroy(neuronID)
    if loadedNeurons[nNBytes].getNbNeurons() == 0:
        del stdParams[nNBytes]
        del loadedNeurons[nNBytes]
