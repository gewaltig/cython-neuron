#!/usr/bin/env bash

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

  