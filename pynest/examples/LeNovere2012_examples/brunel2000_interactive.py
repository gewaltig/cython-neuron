#! /usr/bin/env python

import nest
import nest.raster_plot
import pylab

# Network parameters. These are given in Brunel (2000) J.Comp.Neuro.
g       = 5.0    # Ratio of IPSP to EPSP amplitude: J_I/J_E
eta     = 2.0    # rate of external population in multiples of threshold rate
delay   = 1.5    # synaptic delay in ms
tau_m   = 20.0   # Membrane time constant in mV
V_th    = 20.0   # Spike threshold in mV

N_E = 8000
N_I = 2000
N_neurons = N_E+N_I

C_E    = N_E/10 # number of excitatory synapses per neuron
C_I    = N_I/10 # number of inhibitory synapses per neuron  

J_E  = 0.1
J_I  = -g*J_E

nu_ex  = eta* V_th/(J_E*C_E*tau_m) # rate of an external neuron in ms^-1
p_rate = 1000.0*nu_ex*C_E          # rate of the external population in s^-1


# Set parameters of the NEST simulation kernel
nest.SetKernelStatus({"print_time": True,
                      "local_num_threads": 2})

nest.SetDefaults("iaf_psc_delta", 
                 {"C_m": 1.0,
                  "tau_m": tau_m,
                  "t_ref": 2.0,
                  "E_L": 0.0,
                  "V_th": V_th,
                  "V_reset": 10.0})

nodes   = nest.Create("iaf_psc_delta",N_neurons)
nodes_E= nodes[:N_E]
nodes_I= nodes[N_E:]

nest.CopyModel("static_synapse_hom_wd",
               "excitatory",
               {"weight":J_E, 
                "delay":delay})
nest.RandomConvergentConnect(nodes_E, nodes, C_E,model="excitatory")

nest.CopyModel("static_synapse_hom_wd",
               "inhibitory",
               {"weight":J_I, 
                "delay":delay})
nest.RandomConvergentConnect(nodes_I, nodes, C_I,model="inhibitory")

noise=nest.Create("poisson_generator",1,{"rate": p_rate})
nest.DivergentConnect(noise,nodes,model="excitatory")

spikes=nest.Create("spike_detector",2, 
                   [{"label": "brunel-py-ex"},
                    {"label": "brunel-py-in"}])
spikes_E=spikes[:1]
spikes_I=spikes[1:]

N_rec   = 50    # Number of neurons to record from
nest.ConvergentConnect(nodes_E[:N_rec],spikes_E)
nest.ConvergentConnect(nodes_I[:N_rec],spikes_I)

simtime=300.
nest.Simulate(simtime)

events = nest.GetStatus(spikes,"n_events")

rate_ex= events[0]/simtime*1000.0/N_rec
print "Excitatory rate   : %.2f Hz" % rate_ex

rate_in= events[1]/simtime*1000.0/N_rec
print "Inhibitory rate   : %.2f Hz" % rate_in

nest.raster_plot.from_device(spikes_E, hist=True)
#pylab.show()

