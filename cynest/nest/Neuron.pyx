cdef class Neuron:
    # object used in order to access Time or Scheduler methods.
    # It is overwritten by the kernel in order to make it available.
    cdef object time_scheduler
    # Standard Parameters
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
       
    # We convert the address of the variables into long in order to extract the pointers

    cpdef getPCurrents(self):
        return <long>(&(self.currents))
        
    cpdef getPIn_Spikes(self):
        return <long>(&(self.in_spikes))
        
    cpdef getPEx_Spikes(self):
        return <long>(&(self.ex_spikes))
       
    cpdef getPT_Lag(self):
        return <long>(&(self.t_lag))

    cpdef getPSpike(self):
        return <long>(&(self.spike))

