cdef class SpecialFunctions:
    cdef double (*get_msFct)(int, long, double)
    cdef long (*get_tics_or_stepsFct)(int, int, long, double)
    cdef unsigned int (*get_scheduler_valueFct)(int, unsigned int)
    cdef int neuron_index

    def __cinit__(self):
        neuron_index = 0
        
    def __dealloc__(self):
        pass


cdef SpecialFunctions spFct = SpecialFunctions()

# base class
class Neuron:
    def __init__(self):
        pass

    def __dealloc__(self):
        pass

    def update(self):
        pass

    def calibrate(self):
        pass

    def setStatus(self):
        pass

    def getStatus(self):
        pass

    def get_ms_on_resolution(self):
        return spFct.get_msFct(0, -1, -1)

    def get_ms_on_tics(self, tics):
        return spFct.get_msFct(1, tics, -1)

    def get_ms_on_steps(self, steps):
        return spFct.get_msFct(2, steps, -1)

    def get_tics_on_resolution(self):
        return spFct.get_tics_or_stepsFct(0, 1, -1, -1)

    def get_tics_on_steps(self, steps):
        return spFct.get_tics_or_stepsFct(2, 1, steps, -1)

    def get_tics_on_ms(self, ms):
        return spFct.get_tics_or_stepsFct(3, 1, -1, ms)

    def get_tics_on_ms_stamp(self, ms_stamp):
        return spFct.get_tics_or_stepsFct(4, 1, -1, ms_stamp)

    def get_steps_on_resolution(self):
        return spFct.get_tics_or_stepsFct(0, 2, -1, -1)

    def get_steps_on_tics(self, tics):
        return spFct.get_tics_or_stepsFct(1, 2, tics, -1)

    def get_steps_on_ms(self, ms):
        return spFct.get_tics_or_stepsFct(3, 2, -1, ms)

    def get_steps_on_ms_stamp(self, ms_stamp):
        return spFct.get_tics_or_stepsFct(4, 2, -1, ms_stamp)

    def get_modulo(self, value):
        return spFct.get_scheduler_valueFct(0, value)

    def get_slice_modulo(self, value):
        return spFct.get_scheduler_valueFct(1, value)

    def get_min_delay(self):
        return spFct.get_scheduler_valueFct(2, -1)

    def get_max_delay(self):
        return spFct.get_scheduler_valueFct(3, -1)



# Here should be put the user herited class

# location symbol for anchoring the code : <!f>zg4"*$
# $*"4gz>f!< : location symbol for end of code anchoring

# End of the user herited class




# every neuron on the NEST side as a counterpart here. This avoids creating objects each time
cdef neurons = {}


# sets the special functions used by the neurons
cdef public void putSpecialFunctions(double (*get_ms)(int, long, double), long (*get_tics_or_steps)(int, int, long, double), unsigned int (*get_scheduler_value)(int, unsigned int)) with gil:
    spFct.get_msFct = get_ms
    spFct.get_tics_or_stepsFct = get_tics_or_steps
    spFct.get_scheduler_valueFct = get_scheduler_value


cdef public int neuronExists(int neuronID):
    if neurons.has_key(neuronID):
        return 1
    else:
        return 0

cdef public int getNbNeurons():
    return len(neurons)


cdef public void destroy(int neuronID):
    if neuronExists(neuronID) == 1:
        del neurons[neuronID]

# creation of a new neuron and returning of the corresponding id. Now the id is just the location inside the list,
# but it will be enhanced in order to take into account neuron deletions (in which case two neurons could have the same id)
cdef public int createNeuron() with gil:
# symbol for anchoring (n = ...): <h4Da10làIIg>
# >gIIàl01aD4h< : end of anchoring
    n.spike = 0 # add spike and other standard params
    n.in_spikes = 0.0
    n.ex_spikes = 0.0
    n.currents = 0.0
    n.t_lag = 0
    neurons[spFct.neuron_index] = n
    spFct.neuron_index = spFct.neuron_index + 1
    return spFct.neuron_index - 1


# used in order to set the neuron members
cdef public void setNeuronParams(int neuronID, dict members) with gil:
    for key, value in members.iteritems():
        if (key is not "") and (key is not "calibrate") and (key is not "update"):
            setattr(neurons[neuronID], key, value)

# used in order to get the neuron members
cdef public dict getNeuronParams(int neuronID) with gil:
    output = {}
    for key, value in vars(neurons[neuronID]).iteritems():
        if key.startswith('_') == False and type(value).__name__ in dir(__builtins__):
            output[key] = value

    return output

# Setting and getting the Standard Parameters (during update)
cdef public void setStdVars(int neuronID, long spike, double in_spikes, double ex_spikes, double currents, long lag) with gil:
    neurons[neuronID].spike = spike
    neurons[neuronID].in_spikes = in_spikes
    neurons[neuronID].ex_spikes = ex_spikes
    neurons[neuronID].currents = currents
    neurons[neuronID].t_lag = lag

cdef public void getStdVars(int neuronID, long* spike, double* in_spikes, double* ex_spikes, double* currents, long* lag) with gil:
    spike[0] = neurons[neuronID].spike
    in_spikes[0] = neurons[neuronID].in_spikes
    ex_spikes[0] = neurons[neuronID].ex_spikes
    currents[0] = neurons[neuronID].currents
    lag[0] = neurons[neuronID].t_lag


# Normal callable functions
cdef public void update(int neuronID) with gil:
    neurons[neuronID].update()

cdef public void calibrate(int neuronID) with gil:
    neurons[neuronID].calibrate()

cdef public void setStatus(int neuronID) with gil:
    neurons[neuronID].setStatus()

cdef public void getStatus(int neuronID) with gil:
    neurons[neuronID].getStatus()
