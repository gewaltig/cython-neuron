'''
NEST Topology Module Example

Create layer of 4x3 iaf_neurons, visualize

BCCN Tutorial @ CNS*09
Hans Ekkehard Plesser, UMB/Simula
'''

import nest
import pylab
import nest.topology as topo

nest.ResetKernel()

l1 = topo.CreateLayer({'columns': 4, 'rows': 3, 
                       'extent': [2.0, 1.5],
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

# pylab.savefig('grid_iaf.png')

