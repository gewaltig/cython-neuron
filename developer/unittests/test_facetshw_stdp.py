#!/usr/bin/python

import numpy
import nest

weight_stim      = 150.0  #strong stimulation to evoke one postsynaptic spike per presynaptic spike
                          #'parrot_neurons' are not feasible because of multiplicity > 0
weight_max       = 1.0
weight           = weight_max * 7.0 / (pow(2,4) - 1) #4-bit discrete weight
delay            = 0.1
tau_stdp         = 20.0
a_th             = 1.5    #the spike pair accumulation thresholds are lowered to evoke one facilitation and one depression of the synaptic weight
controller_speed = delay  #the weight update controller frequency is increased for possible weight updates in each simulation step
spike_times_pre  = [1.0, 20.0, 25.0, 1000.0, 2000.0] #"1000.0" to finish spike pair accumulation of spike times < 1000.0;
                                                     #"2000.0" to update weight according to finished spike pair accumulation
spike_times_post = [7.0, 14.0, 26.0]
simulation_break = 1500.0
synapse_model    = 'stdp_facetshw_synapse_hom'

nest.ResetKernel()
nest.SetKernelStatus({'resolution': delay})

neuron_pre  = nest.Create('iaf_cond_alpha')
neuron_post = nest.Create('iaf_cond_alpha')
spikes_pre  = nest.Create('spike_generator')
spikes_post = nest.Create('spike_generator')
spike_detector = nest.Create('spike_detector')

nest.SetStatus(spikes_pre, [{'spike_times': spike_times_pre}])
nest.SetStatus(spikes_post, [{'spike_times': spike_times_post}])
nest.SetStatus(spike_detector, [{'withweight': True}])

nest.SetDefaults(synapse_model, {'driver_readout_time': controller_speed, 'a_th_causal': a_th, 'a_th_acausal': a_th, 'tau_plus': tau_stdp, 'tau_minus_stdp': tau_stdp, 'Wmax': weight_max})

nest.Connect(spikes_pre, neuron_pre, weight_stim, delay)
nest.Connect(spikes_post, neuron_post, weight_stim, delay)
nest.Connect(neuron_pre, spike_detector, weight, delay)
nest.Connect(neuron_post, spike_detector, weight, delay)
nest.Connect(neuron_pre, neuron_post, weight, delay, model=synapse_model) #the plastic hardware inspired synapse

nest.Simulate(simulation_break)

#receive all pre- and postsynaptic spikes
spike_times_out      = nest.GetStatus(spike_detector)[0]['events']['times']
spike_gid_out        = nest.GetStatus(spike_detector)[0]['events']['senders']
spike_times_pre_out  = spike_times_out[spike_gid_out == neuron_pre]
spike_times_post_out = spike_times_out[spike_gid_out == neuron_post]
print 'presynaptic spikes:  ', spike_times_pre_out
print 'postsynaptic spikes: ', spike_times_post_out

#calculate spike pair accumulations and compare them to simulation results
i = 0 #spike_times_pre_out index
j = 0 #spike_times_post_out index
a_causal = 0
a_acausal = 0
while i < len(spike_times_pre_out) and j < len(spike_times_post_out):
  if spike_times_pre_out[i] < spike_times_post_out[j]:
    while i + 1 < len(spike_times_pre_out) and spike_times_pre_out[i + 1] < spike_times_post_out[j]: #spike pairing scheme
      i += 1
    print 'causal ', spike_times_pre_out[i] - (spike_times_post_out[j] + delay)
    a_causal += numpy.exp((spike_times_pre_out[i] - (spike_times_post_out[j] + delay)) / tau_stdp)
    i += 1
  if i < len(spike_times_pre_out) and j < len(spike_times_post_out) and spike_times_pre_out[i] > spike_times_post_out[j]:
    while j + 1 < len(spike_times_post_out) and spike_times_pre_out[i] > spike_times_post_out[j + 1]: #spike pairing scheme
      j += 1
    print 'acausal', (spike_times_post_out[j] + delay) - spike_times_pre_out[i]
    a_acausal += numpy.exp(((spike_times_post_out[j] + delay) - spike_times_pre_out[i]) / tau_stdp)
    j += 1

connection = nest.FindConnections(neuron_pre)
for i in range(len(connection)):
  if nest.GetStatus([connection[i]])[0]['synapse_type'] == synapse_model:
    print 'causal spike pair accumulation: ', nest.GetStatus([connection[i]])[0]['a_causal'], '==', a_causal
    print 'acausal spike pair accumulation:', nest.GetStatus([connection[i]])[0]['a_acausal'], '==', a_acausal
    print 'weights', nest.GetStatus([connection[i]])[0]['weight'], '==', weight

nest.Simulate(numpy.max(numpy.concatenate((spike_times_pre, spike_times_post))) + 1.0)

#calculate possible weight updates
if a_causal > a_th:
  a_causal = 0
  weight += weight_max / (pow(2,4) - 1)

if a_acausal > a_th:
  a_acausal = 0
  weight -= weight_max / (pow(2,4) - 1)

for i in range(len(connection)):
  if nest.GetStatus([connection[i]])[0]['synapse_type'] == synapse_model:
    print 'causal spike pair accumulation: ', nest.GetStatus([connection[i]])[0]['a_causal'], '==', a_causal
    print 'acausal spike pair accumulation:', nest.GetStatus([connection[i]])[0]['a_acausal'], '==', a_acausal
    print 'weights', nest.GetStatus([connection[i]])[0]['weight'], '==', weight

#TODO: add getting and setting of model parameters

#test assignment of synapses to synapse drivers
no_synapses         = 10
synapses_per_driver = 2
spike_times_pre     = [1.0, 10.0]

nest.ResetKernel()
nest.SetKernelStatus({'resolution': delay})

spikes_pre  = nest.Create('spike_generator')
neuron_pre  = nest.Create('parrot_neuron', no_synapses)
neuron_post = nest.Create('iaf_cond_alpha')

nest.SetDefaults(synapse_model, {'synapses_per_driver': synapses_per_driver, 'driver_readout_time': controller_speed, 'a_th_causal': a_th, 'a_th_acausal': a_th, 'tau_plus': tau_stdp, 'tau_minus_stdp': tau_stdp, 'Wmax': weight_max})
nest.SetStatus(spikes_pre, [{'spike_times': spike_times_pre}])

nest.DivergentConnect(spikes_pre, neuron_pre, weight_stim, delay)
nest.ConvergentConnect(neuron_pre, neuron_post, weight, delay, model=synapse_model) #the plastic hardware inspired synapse

nest.Simulate(numpy.max(spike_times_pre))
