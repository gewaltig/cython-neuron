
cdef class NESTEngineContainer:
    cdef object cynest
    cdef object hl_simulate
    cdef object exec_dir

    cpdef setEngine(self, obj):
        self.cynest = obj
        self.hl_simulate = self.cynest.Simulate
        
    cpdef setExec_Dir(self, obj):
         self.exec_dir = obj

cdef NESTEngineContainer nest_engine = NESTEngineContainer()


def setEngine(obj):
    nest_engine.setEngine(obj)

def setExec_Dir(obj):
    nest_engine.setExec_Dir(obj)


# Graphics tools
include "graphics_simulator.pyx"
include "graphics_visualizer.pyx"
# End of graphics tools




def setGraphicsSimulator(s):	
    if type(s) != bool:
        print("Error: The graphics simulator setting value must be a boolean.")
		
    if s == True:
        nest_engine.cynest.Simulate = graphics_simulate
    else:
        nest_engine.cynest.Simulate = nest_engine.hl_simulate


def initNetworkVisualizer():
    pass
