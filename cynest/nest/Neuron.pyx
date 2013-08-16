
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


cdef class ObjectManager:
    cdef object timeObj
    cdef object ticObj
    cdef object stepObj
    cdef object msObj
    cdef object ms_stampObj

    def setTime(self, obj):
        self.timeObj = obj

    def setTic(self, obj):
        self.ticObj = obj

    def setStep(self, obj):
        self.stepObj = obj

    def setMs(self, obj):
        self.msObj = obj

    def setMs_stamp(self, obj):
        self.ms_stampObj = obj


cdef ObjectManager objectManager = ObjectManager()

def setTime(obj):
    objectManager.setTime(obj)

def setTic(obj):
    objectManager.setTic(obj)

def setStep(obj):
    objectManager.setStep(obj)

def setMs(obj):
    objectManager.setMs(obj)

def setMs_stamp(obj):
    objectManager.setMs_stamp(obj)





cdef class Unit:
    cdef object ob



cdef class tic(Unit):
    def __cinit__(self, t):
        self.ob = objectManager.ticObj.create(t)



cdef class step(Unit):
    def __cinit__(self, t):
        self.ob = objectManager.stepObj.create(t)



cdef class ms(Unit):
    def __cinit__(self, t):
        self.ob = objectManager.msObj.create(t)



cdef class ms_stamp(Unit):
    def __cinit__(self, t):
        self.ob = objectManager.ms_stampObj.create(t)



cdef class Time:
    cdef object time_ob

    def __cinit__(self, Unit t):
        self.time_ob = objectManager.timeObj.create(t.ob)

    def get_tics(self):
        return self.time_ob.get_tics()

    def get_steps(self):
        return self.time_ob.get_steps()

    def get_ms(self):
        return self.time_ob.get_ms()



cpdef Time_get_resolution():
    return objectManager.timeObj.get_resolution()



