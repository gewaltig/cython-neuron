"""
Network tests
"""

import unittest
import nest


class NetworkTestCase(unittest.TestCase):

   def test_BeginEndSubnet(self):
       """Begin/End Subnet"""

       nest.ResetKernel()

       nest.BeginSubnet()
       sn = nest.EndSubnet()

       nest.BeginSubnet(label='testlabel')
       sn = nest.EndSubnet()

       self.assertEqual(nest.GetStatus(sn,'label')[0], 'testlabel')
       

   def test_CurrentSubnet(self):
       """Current Subnet"""

       nest.ResetKernel()
       self.assertEqual(nest.CurrentSubnet(), [0])

       nest.BeginSubnet()
       self.assertEqual(nest.CurrentSubnet(), [1])


   def test_GetLeaves(self):
       """GetLeaves"""

       nest.ResetKernel()
       nest.BeginSubnet(label='testlabel')
       n  = nest.Create('iaf_neuron',100)
       sn = nest.EndSubnet()

       self.assertEqual(len(nest.GetLeaves(sn)[0]), len(n))


   def test_GetNetwork(self):
       """GetNetwork"""

       nest.ResetKernel()
       nest.BeginSubnet(label='subnet1')
       nest.BeginSubnet(label='subnet2')       

       n  = nest.Create('iaf_neuron',100)
       sn2 = nest.EndSubnet()
       sn1 = nest.EndSubnet()

       self.assertEqual(nest.CurrentSubnet(), [0])
       self.assertEqual(nest.GetNetwork(sn1,1)[1], sn2[0])
       self.assertEqual(len(nest.GetNetwork(sn1,2)[1]), len(nest.GetNetwork(sn2,1)))


   def test_GetAddres_subnet(self):
      """Subnets"""

      nest.ResetKernel()
      for i in range(10):
         nest.BeginSubnet(str(i))

      self.assertEqual(nest.CurrentSubnet(),[10])
      n=nest.Create('iaf_neuron')
      self.assertEqual(nest.GetAddress(n),[[0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]])
      

def suite():

    suite = unittest.makeSuite(NetworkTestCase,'test')
    return suite


if __name__ == "__main__":

    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(suite())
