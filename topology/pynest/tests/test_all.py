"""
Testsuite for Topology PyNEST Interface.

This testsuite mainly tests the PyNEST interface to the
topology module, not the underlying topology module functions.

It also tests the visualization functions that are available
in PyNEST only.
"""

import unittest

from nest.topology.tests import test_basics
from nest.topology.tests import test_dumping
from nest.topology.tests import test_plotting

def suite():

    suite = unittest.TestSuite()

    suite.addTest(test_basics.suite())
    suite.addTest(test_dumping.suite())
    suite.addTest(test_plotting.suite())
    
    return suite


if __name__ == "__main__":

    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(suite())
