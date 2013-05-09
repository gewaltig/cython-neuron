import math
import sys

class cython_iaf_psc_delta(Neuron):
    def __init__(self):
        self.tau_m   = 10.0  # ms
        self.C_m     = 250.0 # pF
        self.t_ref   =  2.0 # ms
        self.E_L     = -70.0 # mV
        self.I_e_     = 0.0 # pA
        self.V_th    = -55.0 - self.E_L # mV, rel to U0_
        self.V_min_   = -sys.float_info.max # relative U0_-55.0-U0_),  // mV, rel to U0_
        self.V_reset = -70.0 - self.E_L
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

    def calibrate(self):
        self.ms_resolution = self.get_ms_on_resolution()
        self.P33_ = math.exp(-self.ms_resolution/self.tau_m)
        self.P30_ = 1/self.C_m*(1-self.P33_)*self.tau_m
        self.RefractoryCounts_ = self.get_steps_on_ms(self.t_ref)

    def setStatus(self):
        # if U0_ is changed, we need to adjust all variables defined relative to U0_
        ELold = self.E_L
        delta_EL = self.E_L - ELold
        # ...normally should go on, but useless for the purpose of the simulation



    def update(self):
        if self.r_ == 0:
            # neuron not refractory
            self.y3_ = self.P30_*(self.y0_ + self.I_e_) + self.P33_*self.y3_ + (self.ex_spikes + self.in_spikes)

            # if we have accumulated spikes from refractory period, 
            # add and reset accumulator
            if self.with_refr_input_ and self.refr_spikes_buffer_ != 0.0:
                self.y3_ += self.refr_spikes_buffer_
                self.refr_spikes_buffer_ = 0.0
      
            # lower bound of membrane potential
            if self.y3_ < self.V_min_:
                self.y3_ = self.V_min_	 
    
        else: # neuron is absolute refractory
            # read spikes from buffer and accumulate them, discounting
            # for decay until end of refractory period
            if self.with_refr_input_:
                self.refr_spikes_buffer_ += (self.ex_spikes + self.in_spikes) * math.exp(-self.r_ * self.ms_resolution / self.tau_m)
            else:
                self.ex_spikes = 0  # clear buffer entry, ignore spike
                self.in_spikes = 0

            self.r_ -= 1
   
        # threshold crossing
        if self.y3_ >= self.V_th:
            self.r_ = self.RefractoryCounts_
            self.y3_ = self.V_reset
            self.spike = 1 # True
        else:
            self.spike = 0 # False
