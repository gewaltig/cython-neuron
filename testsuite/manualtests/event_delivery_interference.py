#!/usr/bin/python

"""
Event interference during spike delivery.

This test demonstrates that if the send() method of a Connection
manipulates the multiplicity of the Event object that is used
for all spike deliveries during a call to Scheduler::deliver_events(),
errors results, as this changed multiplicity will affect all 
subsequent deliveries during the same time step.

See also #426.

This test requires static_synapse_mult0 from developer.

Originally reported by Thomas Pfeil.
This test by Hans Ekkehard Plesser, 2010-05-06.
"""

import nest

nest.ResetKernel()
nest.SetKernelStatus({'print_time': True, 'resolution': 0.1})

# create spike generator that sends two spikes in a min_delay period
sg = nest.Create('spike_generator', params={'precise_times': false,
                                            'spike_times': [0.2, 0.3]})
a = nest.Create('parrot_neuron')
c = nest.Create('parrot_neuron')
spike_det_a = nest.Create('spike_detector')

nest.Connect(sg, a)   
nest.Connect(a, spike_det_a)   # spike_detector fails on multiplicity 0
nest.Connect(a, c, model='static_synapse_mult0')  # sets event multiplicity to 0

nest.Simulate(3.0)
