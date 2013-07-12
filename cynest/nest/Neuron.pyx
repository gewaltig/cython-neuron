cdef class Neuron:
    cdef object time_scheduler
    cdef double currents
    cdef double in_spikes
    cdef double ex_spikes
    cdef long t_lag
    cdef long spike
    
    def __cinit__(self):
        pass

    cpdef calibrate(self):
        pass
        
    cpdef update(self):
        pass
        
    cpdef getStatus(self):
        return {}
        
    cpdef setStatus(self, params):
        pass
        
    cpdef setTimeScheduler(self, ts):
        self.time_scheduler = ts
        
    cpdef getSpike(self):
        return self.spike
    
    cpdef setStdParams(self, curr, in_s, ex_s, t_l):
        self.currents = curr
        self.in_spikes = in_s
        self.ex_spikes = ex_s
        self.t_lag = t_l
