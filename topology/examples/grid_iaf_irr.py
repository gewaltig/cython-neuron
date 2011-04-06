'''
NEST Topology Module Example

Create layer of 12 freely placed iaf_neurons, visualize

BCCN Tutorial @ CNS*09
Hans Ekkehard Plesser, UMB/Simula
'''

import nest
import pylab
import random
import nest.topology as topo

nest.ResetKernel()

# generate list of 12 (x,y) pairs
pos = [[random.uniform(-0.75,0.75), random.uniform(-0.5,0.5)]
       for j in range(12)]

l1 = topo.CreateLayer({'extent': [2., 1.5],
                       'positions': pos, 
                       'elements': 'iaf_neuron'})

nest.PrintNetwork()
nest.PrintNetwork(2)
nest.PrintNetwork(2, l1)

topo.PlotLayer(l1, nodesize=50)

# beautify
pylab.axis([-1.0, 1.0, -0.75, 0.75])
pylab.axes().set_aspect('equal', 'box')
pylab.axes().set_xticks((-0.75, -0.25, 0.25, 0.75))
pylab.axes().set_yticks((-0.5, 0, 0.5))
pylab.grid(True)
pylab.xlabel('4 Columns, Extent: 1.5')
pylab.ylabel('2 Rows, Extent: 1.0')

# pylab.savefig('grid_iaf_irr.png')
