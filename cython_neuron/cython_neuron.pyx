# base class
class Neuron:
    def __cinit__(self):
        self._nMembers = {}
        pass

    def __dealloc__(self):
        pass

    def update(self):
        print "Updating the neuron!"
        self.currents = self.currents + 1.0
        self.t_lag = self.t_lag + 2.0

    def calibrate(self):
        print "Calibrating the neuron!"
        self.currents = 1500.0


# every neuron on the NEST side as a counterpart here. This is important because the user neuron can have private fields,
# which can't be saved in the parameters object (maybe they aren't double values)
neurons = []


# creation of a new neuron and returning of the corresponding id. Now the id is just the location inside the list,
# but it will be enhanced in order to take into account neuron deletions (in which case two neurons could have the same id)
cdef public int createNeuron() with gil:
    neurons.append(Neuron())
    return len(neurons) - 1

# function which takes a char buffer and constructs the corresponding python string. This is not exactly the same
# function as in dynamicneuronssync.pyx
cdef bytes transformIntoString(char* source) with gil:
    cdef int i = 0
    output = ""

    while source[i] is not '\0':
        output = output + unichr(source[i])
        i = i + 1
    return output.encode("UTF-8")

# function which takes a python string and a buffer and fills the buffer with the string characters
cdef void constructString(source, char* dest) with gil:
    cdef int i = 0

    for ch in source:
        if i < 99:
            dest[i] = ord(ch)
            i = i + 1
    dest[i] = '\0'



# used in order to set a neuron member
cdef public void setNeuronParam(int neuronID, char* keyC, double value) with gil:
    key = transformIntoString(keyC)
    if key is not "".encode("UTF-8"):
        exec("neurons[neuronID]." + key + " = " + str(value))


# this is a special function couples with the two just below
# returns the current neuron parameter
cdef public double getNeuronParam(int neuronID, char* param) with gil:
    for m in neurons[neuronID]._nMembers:
        if not m.startswith("_"):
            value = neurons[neuronID]._nMembers[m]
            constructString(m, param)
            neurons[neuronID]._nMembers.pop(m)
            return value

# this method is coupled with the other two around it. It initializes the iterator
cdef public void initIteratorNeuronParams(int neuronID) with gil:
     neurons[neuronID]._nMembers = vars(neurons[neuronID]).copy()

# this is a special function couples with the two just above
# it updates the iterator and sets the current parameter to the next one
cdef public int hasNeuronNextParam(int neuronID) with gil:
    if len(neurons[neuronID]._nMembers) > 0:
        return 1
    else:
        return 0


cdef public void update(int neuronID) with gil:
        neurons[neuronID].update()

cdef public void calibrate(int neuronID) with gil:
        neurons[neuronID].calibrate()

