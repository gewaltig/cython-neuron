#!/bin/bash

DATADIR="./data"
K_SYNAPSES=0

cd $DATADIR

for N_NEURONS in 16000 48000 96000
do
  # the LCM of the process numbers below is 1440
  for N_PROC in 4 8 12 16 24 32 48 64 # 80 96 128 160
  do

  # data file
  DAT_NAME="n"$N_PROC"_N"$N_NEURONS"_K"$K_SYNAPSES".dat"

  # output file from queue
  BATCH_NAME="batch_n"$N_PROC"_N"$N_NEURONS"_K"$K_SYNAPSES".sh.o*"

  # filter out MEM info
  cat $BATCH_NAME | grep MEM | cut -f 2,3,4 > $DAT_NAME
  done
done

exit 0