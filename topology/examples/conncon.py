'''
NEST Topology Module Example

Create two 30x30 layers of iaf_neurons, 
connect with convergent projection and rectangular mask,
visualize connection from target perspective. 

BCCN Tutorial @ CNS*09
Hans Ekkehard Plesser, UMB/Simula
'''

import nest, pylab
import nest.topology as topo
nest.ResetKernel()

# create two test layers
a = topo.CreateLayer({'columns': 30, 'rows': 30, 'extent': [3.0, 3.0],
                      'elements': 'iaf_neuron', 'edge_wrap': True})
b = topo.CreateLayer({'columns': 30, 'rows': 30, 'extent': [3.0, 3.0],
                      'elements': 'iaf_neuron', 'edge_wrap': True})

topo.ConnectLayers(a, b, {'connection_type': 'convergent',
                         'mask': {'rectangular': {'lower_left': [-0.2, -0.5], 'upper_right': [0.2, 0.5]}},
                         'kernel': 0.5,
                         'weights': {'uniform': {'min': 0.5, 'max': 2.0}},
                         'delays': 1.0})

pylab.clf()

# plot sources of neurons in different grid locations
for ctr in [[15,15],[0,0]]:
    
    # obtain node id for center
    ctr_id = topo.GetElement(b, ctr)

    # obtain list of outgoing connections for ctr,
    # assume static_synapse
    spos = zip(*[topo.GetPosition([n])[0] for n in nest.GetLeaves(a)[0]
                 if nest.GetStatus(nest.FindConnections([n]), 'target').count(ctr_id[0]) > 0])

    # scatter-plot
    pylab.scatter(spos[0], spos[1], 20, zorder = 10)

    # mark sender position with transparent red circle
    ctrpos = pylab.array(topo.GetPosition(ctr_id)[0])
    pylab.gca().add_patch(pylab.Circle(ctrpos, radius=0.1, zorder = 99,
                                       fc = 'r', alpha = 0.4, ec = 'none'))
 
    # mark mask position with open red rectangle
    pylab.gca().add_patch(pylab.Rectangle(ctrpos-(0.2,0.5), 0.4, 1.0, zorder = 1,
                                       fc = 'none', ec = 'r', lw=3))

# mark layer edge
pylab.gca().add_patch(pylab.Rectangle((-1.5,-1.5), 3.0, 3.0, zorder = 1,
                                      fc = 'none', ec = 'k', lw=3))

# beautify
pylab.axes().set_xticks(pylab.arange(-1.5, 1.55, 0.5))
pylab.axes().set_yticks(pylab.arange(-1.5, 1.55, 0.5))
pylab.grid(True)
pylab.axis([-2.0, 2.0, -2.0, 2.0])
pylab.axes().set_aspect('equal', 'box')
pylab.title('Connection sources')

# pylab.savefig('conncon.pdf')
