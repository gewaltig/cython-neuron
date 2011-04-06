'''
NEST Topology Module Example

Create three layers of 4x3 iaf_neurons, each with different center.

BCCN Tutorial @ CNS*09
Hans Ekkehard Plesser, UMB/Simula
'''

import pylab
import time
import nest
import nest.topology as topo

for ctr in [(0.0,0.0),(-2.0,2.0),(0.5,1.0)]:
    nest.ResetKernel()
    pylab.clf()
    l1 = topo.CreateLayer({'columns': 4, 'rows': 3, 
                           'extent': [2.0, 1.5],
                           'center': ctr,
                           'elements': 'iaf_neuron'})
    
    topo.PlotLayer(l1, nodesize=50, fig=pylab.gcf())

    # beautify
    pylab.axis([-3, 3, -3, 3])
    pylab.axes().set_aspect('equal', 'box')
    pylab.axes().set_xticks(pylab.arange(-3.0, 3.1, 1.0))
    pylab.axes().set_yticks(pylab.arange(-3.0, 3.1, 1.0))
    pylab.grid(True)
    pylab.xlabel('4 Columns, Extent: 1.5, Center: %.1f' % ctr[0])
    pylab.ylabel('2 Rows, Extent: 1.0, Center: %.1f' % ctr[1])
    
    pylab.draw()
    time.sleep(1) 

    # pylab.savefig('grid_iaf_oc_%.1f_%.1f.png' % ctr)
