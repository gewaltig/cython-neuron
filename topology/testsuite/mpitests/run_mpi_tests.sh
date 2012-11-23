#!/usr/bin/env bash
#
# run_mpi_tests.sh
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

# This script runs both mpi tests, compares their output and reports.
# It should only report equal files.

# PYTHONPATH must be set appropriately

# remove old output
rm -rf conv div

# create directories
mkdir -p conv/{1,2,4} div/{1,2,4}

# run tests
for dir in 'conv' 'div'
do
  cd $dir
  for n in 1 2 4 
  do
    cd $n
    mpirun -np $n python ../../topo_mpi_test_${dir}.py > /dev/null
    cd ..
  done
  diff -qs 1 2
  diff -qs 1 4
  cd ..
done

# remove output
rm -rf conv div


# run regression test for #516 --- will hang on failure
mpirun -np 6 python ticket-516.py
  