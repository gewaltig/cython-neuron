#! /usr/bin/env python
#
# test_all.py
#
# This file is part of NEST.
#
# Copyright (C) 2004 The NEST Initiative
#
# NEST is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# NEST is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEST.  If not, see <http://www.gnu.org/licenses/>.
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
