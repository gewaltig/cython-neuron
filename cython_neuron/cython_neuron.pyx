import sys

# base class
class Neuron:
    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    def update(self):
        pass

    def calibrate(self):
        pass


# Here should be put the user herited class

# location symbol for anchoring the code : <!f>zg4"*$


class MyNeuron(Neuron):
    def __cinit__(self):
        self.tau_m_   = 10.0  # ms
        self.c_m_     = 250.0 # pF
        self.t_ref_   =  2.0 # ms
        self.E_L_     = -70.0 # mV
        self.I_e_     = 0.0 # pA
        self.V_th_    = -55.0 - self.E_L_ # mV, rel to U0_
        self.V_min_   = -sys.floatinfo.max # relative U0_-55.0-U0_),  // mV, rel to U0_
        self.V_reset_ = -70.0 - self.E_L_
        self.with_refr_input_ = False
        self.y0_      = 0.0
        self.y3_      = 0.0
        self.r_       = 0
        self.refr_spikes_buffer_ = 0.0
        self.P30_ = 0.0
        self.P33_ = 0.0
        self.RefractoryCounts_ = 0

    def __dealloc__(self):
        pass

    def update(self):
        pass

    def calibrate(self):
        pass

# $*"4gz>f!< : location symbol for end of code anchoring

# End of the user herited class




# every neuron on the NEST side as a counterpart here. This is important because the user neuron could have private fields
neurons = []


# sets the special functions used by the neurons
cdef public void putSpecialFunctions(double (*get_ms)(int, long, double), long (*get_tics_or_steps)(int, int, long, double), unsigned int (*get_scheduler_value)(int, unsigned int)) with gil:
    pass


# creation of a new neuron and returning of the corresponding id. Now the id is just the location inside the list,
# but it will be enhanced in order to take into account neuron deletions (in which case two neurons could have the same id)
cdef public int createNeuron() with gil:
    neurons.append(MyNeuron())
    return len(neurons) - 1


# used in order to set the neuron members
cdef public void setNeuronParams(int neuronID, dict members) with gil:
    for key, value in members.iteritems():
        if (key is not "") and (key is not "calibrate") and (key is not "update"):
            setattr(neurons[neuronID], key, value)

# used in order to get the neuron members
cdef public dict getNeuronParams(int neuronID) with gil:
    return vars(neurons[neuronID])

cdef public void update(int neuronID) with gil:
        neurons[neuronID].update()

cdef public void calibrate(int neuronID) with gil:
        neurons[neuronID].calibrate()

