'''
NEST Topology Module

EXPERIMENTAL example of 3d layer.

3d layers are currently not supported, use at your own risk!

Hans Ekkehard Plesser, UMB
'''

import nest
import pylab
import random
import nest.topology as topo
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

nest.ResetKernel()

# generate list of 100 (x,y,z) pairs
pos = [[random.uniform(-0.5,0.5), random.uniform(-0.5,0.5), random.uniform(-0.5,0.5)]
       for j in range(10000)]

l1 = topo.CreateLayer({'extent': [1.5, 1.5, 1.5], # must specify 3d extent AND center
                       'center': [0., 0., 0.],
                       'positions': pos,
                       'elements': 'iaf_neuron'})

# visualize
#xext, yext = nest.GetStatus(l1, 'topology')[0]['extent']
#xctr, yctr = nest.GetStatus(l1, 'topology')[0]['center']

# extract position information, transpose to list of x, y and z positions
xpos, ypos, zpos = zip(*topo.GetPosition(nest.GetChildren(l1)))
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(xpos, ypos, zpos, s=15, facecolor='b', edgecolor='none')

# Gaussian connections in full volume [-0.75,0.75]**3
topo.ConnectLayers(l1, l1,
                   {'connection_type': 'divergent', 'allow_autapses': False,
                    'mask': {'volume': {'lower_left': [-0.75,-0.75,-0.75], 'upper_right': [0.75,0.75,0.75]}},
                    'kernel':{'gaussian': {'p_center': 1., 'sigma': 0.25}}})

# show connections from center element
# sender shown in red, targets in green
ctr=topo.FindCenterElement(l1)
xtgt, ytgt, ztgt = zip(*topo.GetTargetPositions(ctr,l1)[0])
xctr, yctr, zctr = topo.GetPosition(ctr)[0]
ax.scatter([xctr],[yctr],[zctr],s=40, facecolor='r', edgecolor='none')
ax.scatter(xtgt,ytgt,ztgt,s=40, facecolor='g', edgecolor='g')

tgts=topo.GetTargetNodes(ctr,l1)[0]
d=topo.Distance(ctr,tgts)
plt.figure()
plt.hist(d,100)
plt.show()
