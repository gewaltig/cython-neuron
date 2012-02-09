'''
Created on Feb 7, 2012

@author: thhe
'''
import numpy as np
import matplotlib.pyplot as plt
import nest

def step(t, n, initial, after, seed=1, dt=0.05):
    """Simulates for n generators for t ms. Step at T/2."""

    ## prepare/reset nest
    nest.ResetKernel()
    ## initialize simulation
    #np.random.seed(256 * seed)
    nest.SetStatus([0],[{"resolution": dt}])
    nest.SetStatus([0],[{"grng_seed": 256 * seed + 1}])
    nest.SetStatus([0],[{"rng_seeds": [256 * seed + 2]}])    


    model = 'ac_gamma_generator'

    g = nest.Create(model, n, params=initial)
    sd = nest.Create('spike_detector')
    nest.ConvergentConnect(g, sd)
    nest.Simulate(t/2)
    nest.SetStatus(g, after)
    nest.Simulate(t/2)

    return nest.GetStatus(sd, 'events')[0]

def plot_hist(spikes):
    plt.figure()
    plt.hist(spikes['times'], bins=np.arange(0.,max(spikes['times'])+1.5,1.), histtype='step')

if __name__ == '__main__':
    plt.ion()
    spikes = step(1000, 1000,
    {'order': 6.0, 'dc': 60.0, 'ac': 60., 'freq': 10., 'phi': 0.},
    {'order': 6.0, 'dc': 60.0, 'ac': 60., 'freq': 10., 'phi': np.pi},
                  seed=123, dt=1.)
    plot_hist(spikes)
