#!/usr/bin/env python

#  multimeter.py
#
#  Copyright (C) 2009-2010 The NEST Initiative

'''
This file illustrates recording from a iaf_cond_alpha neuron 
using a multimeter.

See multimeter_file.py for an example of how to record to file.
'''

import cynest as nest
import numpy as np
import pylab as pl

nest.ResetKernel()

# display recordables for illustration
print 'iaf_cond_alpha recordables: ', nest.GetDefaults('iaf_cond_alpha')['recordables']

# create neuron and multimeter
n = nest.Create('iaf_cond_alpha', 
                params = {'tau_syn_ex': 1.0, 'V_reset': -70.0})

m = nest.Create('multimeter',
                params = {'withtime': True, 
                          'interval': 0.1,
                          'record_from': ['V_m', 'g_ex', 'g_in']})

# Create spike generators and connect
gex = nest.Create('spike_generator',
                  params = {'spike_times': np.array([10.0, 20.0, 50.0])})
gin = nest.Create('spike_generator',
                  params = {'spike_times': np.array([15.0, 25.0, 55.0])})

nest.Connect(gex, n, params={'weight':  40.0}) # excitatory
nest.Connect(gin, n, params={'weight': -20.0}) # inhibitory
nest.Connect(m, n)

# simulate
nest.Simulate(100)

# obtain and display data
events = nest.GetStatus(m)[0]['events']
t = events['times'];

pl.clf()

pl.subplot(211)
pl.plot(t, events['V_m'])
pl.axis([0, 100, -75, -53])
pl.ylabel('Membrane potential [mV]')

pl.subplot(212)
pl.plot(t, events['g_ex'], t, events['g_in'])
pl.axis([0, 100, 0, 45])
pl.xlabel('Time [ms]')
pl.ylabel('Synaptic conductance [nS]')
pl.legend(('g_exc', 'g_inh'))

pl.show()

