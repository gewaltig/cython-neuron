cdef class CyNeuron:
    cdef object time_scheduler
    cdef double currents
    cdef double in_spikes
    cdef double ex_spikes
    cdef long t_lag
    cdef long spike
    
    def __cinit__(self):
        pass

    cpdef int calibrate(self):
        return 0
        
    cpdef int update(self):
        return 0
        
    cpdef dict getStatus(self):
        return {}
        
    cpdef int setStatus(self, dict params):
        return 0
        
    cpdef int setTimeScheduler(self, object ts):
        self.time_scheduler = ts
        return 0
        
    cpdef long getSpike(self):
        return self.spike
    
    cpdef int setStdParams(self, double curr, double in_s, double ex_s, long t_l):
        self.currents = curr
        self.in_spikes = in_s
        self.ex_spikes = ex_s
        self.t_lag = t_l
        return 0


class PyNeuron:    
    def __init__(self):
        self.time_scheduler = None
        self.currents = 0.0
        self.in_spikes = 0.0
        self.ex_spikes = 0.0
        self.t_lag = 0
        self.spike = 0

    def calibrate(self):
        pass
        
    def update(self):
        pass

    def getStatus(self):
        output = {}
        for key, value in vars(self).iteritems():
            if key.startswith('_') == False and type(value).__name__ in dir(__builtins__):
                output[key] = value

        return output
        
    def setStatus(self, params):
        for key, value in params.iteritems():
            setattr(self, key, value)
        
    def setTimeScheduler(self, ts):
        self.time_scheduler = ts
        
    def getSpike(self):
        return self.spike
    
    def setStdParams(self, curr, in_s, ex_s, t_l):
        self.currents = curr
        self.in_spikes = in_s
        self.ex_spikes = ex_s
        self.t_lag = t_l
        
