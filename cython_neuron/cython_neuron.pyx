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


# used in order to set the neuron members
cdef public void setNeuronParams(int neuronID, dict members) with gil:
    for key in members:
        if (key is not "") and (key is not "calibrate") and (key is not "update"):
            exec("neurons[neuronID]." + key + " = members[key]")


# used in order to get the neuron members
cdef public dict getNeuronParams(int neuronID) with gil:
    return vars(neurons[neuronID])



cdef public void update(int neuronID) with gil:
        neurons[neuronID].update()

cdef public void calibrate(int neuronID) with gil:
        neurons[neuronID].calibrate()

