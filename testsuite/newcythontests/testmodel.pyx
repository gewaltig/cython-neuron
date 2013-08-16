include "/home/jonny/Programs/Nest/include/Neuron.pyx"

class C:
    def __init__(self):
        self.p1 = 1.0
        self.p2 = 250.0


cdef class testmodel(Neuron):
    cdef object params

    def __cinit__(self):
        self.params = C()

    cpdef calibrate(self):
        print Time_get_ms_per_tic()


    cpdef update(self):
        self.params.p1 = self.params.p1 + self.ex_spikes + self.in_spikes + 1.0
        if self.params.p1 >= self.params.p2:
            self.spike = 1 # True
            self.params.p1 = 0.0
        else:
            self.spike = 0 # False

