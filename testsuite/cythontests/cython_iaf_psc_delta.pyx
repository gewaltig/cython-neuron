cdef class Neuron:
    cdef dict params
    cdef object time_scheduler
    
    def __cinit__(self):
        self.params = {}
        self.params['currents'] = 0.0
        self.params['ex_spikes'] = 0.0
        self.params['in_spikes'] = 0.0
        self.params['t_lag'] = 0
        self.params['spike'] = 0

    cpdef calibrate(self):
        pass
        
    cpdef update(self):
        pass
        
    cpdef setStatus(self):
        pass
        
    cpdef getStatus(self):
        pass
        
    cpdef getParams(self):
        return self.params
        
    cpdef setTimeScheduler(self, ts):
        self.time_scheduler = ts


import math
import sys

cdef class cython_iaf_psc_delta(Neuron):
    cdef double ms_resolution
    def __cinit__(self):
        self.params['tau_m']   = 10.0  # ms
        self.params['C_m']     = 250.0 # pF
        self.params['t_ref']   =  2.0 # ms
        self.params['E_L']     = -70.0 # mV
        self.params['I_e_']     = 0.0 # pA
        self.params['V_th']    = -55.0 - self.params['E_L']  # mV, rel to U0_
        self.params['V_min_']   = -sys.float_info.max # relative U0_-55.0-U0_),  // mV, rel to U0_
        self.params['V_reset'] = -70.0 - self.params['E_L'] 
        self.params['with_refr_input_'] = False
        self.params['y0_']      = 0.0
        self.params['y3_']     = 0.0
        self.params['r_']       = 0
        self.params['refr_spikes_buffer_'] = 0.0
        self.params['P30_'] = 0.0
        self.params['P33_'] = 0.0
        self.params['RefractoryCounts_'] = 0

    cpdef calibrate(self):
        self.ms_resolution = self.time_scheduler.get_ms_on_resolution()
        self.params['P33_'] = math.exp(-self.ms_resolution/self.params['tau_m'])
        self.params['P30_'] = 1/self.params['C_m']*(1-self.params['P33_'])*self.params['tau_m']
        self.params['RefractoryCounts_'] = self.time_scheduler.get_steps_on_ms(self.params['t_ref'])

    cpdef setStatus(self):
        pass
        # if U0_ is changed, we need to adjust all variables defined relative to U0_
        Lold = self.params['E_L']
        #delta_EL = self.params['E_L - ELold
        # ...normally should go on, but useless for the purpose of the simulation



    cpdef update(self):
        if self.params['r_'] == 0:
            # neuron not refractory
            self.params['y3_'] = self.params['P30_']*(self.params['y0_'] + self.params['I_e_']) + self.params['P33_']*self.params['y3_'] + (self.params['ex_spikes'] + self.params['in_spikes'])

            # if we have accumulated spikes from refractory period, 
            # add and reset accumulator
            if self.params['with_refr_input_'] and self.params['refr_spikes_buffer_'] != 0.0:
                self.params['y3_'] += self.params['refr_spikes_buffer_']
                self.params['refr_spikes_buffer_'] = 0.0
      
            # lower bound of membrane potential
            if self.params['y3_'] < self.params['V_min_']:
                self.params['y3_'] = self.params['V_min_']	 
    
        else: # neuron is absolute refractory
            # read spikes from buffer and accumulate them, discounting
            # for decay until end of refractory period
            if self.params['with_refr_input_']:
                self.params['refr_spikes_buffer_'] += (self.params['ex_spikes'] + self.params['in_spikes']) * math.exp(-self.params['r_'] * self.ms_resolution / self.params['tau_m'])
            else:
                self.params['ex_spikes'] = 0  # clear buffer entry, ignore spike
                self.params['in_spikes'] = 0

            self.params['r_'] -= 1
   
        # threshold crossing
        if self.params['y3_'] >= self.params['V_th']:
            self.params['r_'] = self.params['RefractoryCounts_']
            self.params['y3_'] = self.params['V_reset']
            self.params['spike'] = 1 # True
        else:
            self.params['spike'] = 0 # False
