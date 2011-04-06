"""
Test of events
"""

import unittest
import nest


class EventsTestCase(unittest.TestCase):
    """Tests of the Connect API"""

    def test_Events_1(self):
        """Recorder Events"""

        nest.ResetKernel()
        
        sd = nest.Create('spike_detector',1,{'withtime':True})
        d  = nest.GetStatus(sd,'events')[0]

        senders = d['senders']
        times   = d['times']  

        vm = nest.Create('voltmeter',1,{'withtime':True})
        d  = nest.GetStatus(vm,'events')[0]

        senders = d['V_m']
        times   = d['times']  


    def test_EventsVoltage(self):
        """Voltage Events"""

        nest.ResetKernel()

        nest.sr('20 setverbosity')
        n  = nest.Create('iaf_neuron')
        vm = nest.Create('voltmeter',1,{'withtime':True, 'interval':1.})        

        nest.Connect(vm,n)
        nest.SetKernelStatus({'print_time':False})
        nest.Simulate(10)

        d  = nest.GetStatus(vm,'events')[0]

        self.assertEqual(len(d['V_m']), 9)
        

    def test_EventsSpikes(self):
        """Spike Events"""

        nest.ResetKernel()

        nest.sr('20 setverbosity')
        
        n  = nest.Create('iaf_neuron',1,{'I_e':1000.})        
        sd = nest.Create('spike_detector',1,{'withtime':True})        

        nest.Connect(n,sd)
        nest.SetKernelStatus({'print_time':False})
        nest.Simulate(1000)

        d  = nest.GetStatus(sd,'events')[0]

        self.assert_(len(d['times'])>0)
        


def suite():

    suite = unittest.makeSuite(EventsTestCase,'test')
    return suite


if __name__ == "__main__":

    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(suite())
