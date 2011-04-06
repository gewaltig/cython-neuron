"""
FindConnections
"""

import unittest
import nest

class FindConnectionsTestCase(unittest.TestCase):
    """Find connections and test if values can be set."""


    def test_FindConnections(self):
        """FindConnections"""

        nest.ResetKernel()
        
        a=nest.Create("iaf_neuron", 3)
        nest.DivergentConnect(a,a)
        c1=nest.FindConnections(a)
        c2=nest.FindConnections(a, synapse_type="static_synapse")
        self.assertEqual(c1, c2)
        
        d1=[{"weight": w} for w in [2.0, 3.0, 4.0]]

        c3=nest.FindConnections(a, a)
        nest.SetStatus(c3, d1)
        s1=nest.GetStatus(c3, "weight")
        self.assertEqual(s1, [w["weight"] for w in d1])


def suite():

    suite = unittest.makeSuite(FindConnectionsTestCase,'test')
    return suite


if __name__ == "__main__":

    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(suite())
