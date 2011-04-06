"""
Topology MPI Test.

This test builds a network using distance-dependent mask and weight function
and writes node and connection information to file.

When run with 1, 2, or 4 MPI processes, identical network structures must result.

Create one subdir per number of MPI processes, then move into each subdir, run there.
Afterwards, diff subdirs. Diff should output nothing.



Hans Ekkehard Plesser, 2010-11-03
"""

import nest 
import nest.topology as topo
import os

nest.sli_run('M_ERROR setverbosity')
nest.SetKernelStatus({'total_num_virtual_procs': 4})

l1 = topo.CreateLayer({'rows': 10,
                       'columns': 20,
                       'elements': ['iaf_neuron', 2],
                       'edge_wrap': True})

l2 = topo.CreateLayer({'rows': 10,
                       'columns': 20,
                       'elements': ['iaf_neuron', 2],
                       'edge_wrap': True})

topo.ConnectLayers(l1, l2, {'connection_type': 'convergent',
                            'mask': {'circular': {'radius': 0.4}},
                            'weights': {'linear': {'c': 1., 'a': -5.}}})

topo.DumpLayerNodes(l1+l2, 'topo_mpi_test.lyr' )
topo.DumpLayerConnections(l1, 'static_synapse', 'topo_mpi_test.cnn_tmp')

# combine all connection files into one sorted file
if nest.Rank() == 0:
    os.system('cat *.cnn_tmp | sort > all_sorted.cnn')
    os.system('rm *.cnn_tmp')

# directories for any number of MPI processes should now be diff-able    