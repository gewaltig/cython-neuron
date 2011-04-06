"""
Stack tests
"""

import unittest
import nest


class StackTestCase(unittest.TestCase):
    """Stack tests"""

    def test_Count(self):
        """Object count"""

        nest.ResetKernel()
        nest.sr('clear')
        
        for i in range(100):
            nest.sps(i)

        nest.sr('count')
        self.assertEqual(nest.spp(), 100)

        for i in range(100):
            self.assertEqual(nest.spp(), (99-i))
        
        nest.sr('count')
        self.assertEqual(nest.spp(), 0)



    def test_PushPop(self):
        """Object push and pop"""

        nest.ResetKernel()
        
        objects = [ 1,           # int
                    3.14,        # double
                    1e-4,        # double
                    -100,        # negative
                    'string',    # string
                    {'key':123}, # dict
                    [1,2,3,4,5], # list
                    ]

        for o in objects:
            nest.sps(o)
            self.assertEqual(o, nest.spp())

        try:
            import numpy
            arr = numpy.array([[1,2,3,4,5],[6,7,8,9,0]])
            nest.sps(arr[1,:])
            self.assert_( (nest.spp() == numpy.array([6, 7, 8, 9, 0])).all())
            nest.sps(arr[:,1])
            self.assert_( (nest.spp() == numpy.array([2, 7])).all())
        except ImportError:
            pass # numpy's not required for pynest to work


def suite():

    suite = unittest.makeSuite(StackTestCase,'test')
    return suite


if __name__ == "__main__":

    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(suite())
