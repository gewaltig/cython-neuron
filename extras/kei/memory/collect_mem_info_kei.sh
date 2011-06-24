#!/bin/bash

dat_dir="./data_meminfo/"
cd $dat_dir
pwd

for M in 192 384 768 1536 3072 6144
do
  # determine number of nodes required (8 cores/node)
  N_NODES=$(($M/8))

  K=0
  for N in 24576 49152 98304 196608 393216 786432 1572864
  do
    # name of submission script for Kei queue
    job_name="mem_params_M"$M"_N"$N"_K"$K
    job_script=$job_name".sh.o*"
    info_file=$job_name".sh.i*"
    dat_name=$job_name".dat"

    # there might be several files due to several runs
    # so we cannot use -f to check existence of file
    ls $info_file
    if [ $? = 0 ]
    then
      # filter out MEM info
      #cat $job_script | grep MEM | cut -f 2,3,4 > $dat_name
      cat $job_script | grep -e "MEM[^I]" > $dat_name
    else
      echo "missing job "$jobname
    fi

  done

  N=98304
  for K in 128 256 512 1024 2048 4096 8192
  do
    # name of submission script for Kei queue
    job_name="mem_params_M"$M"_N"$N"_K"$K
    job_script=$job_name".sh.o*"
    info_file=$job_name".sh.i*"
    dat_name=$job_name".dat"

    ls $info_file
    if [ $? = 0 ]
    then
      # filter out MEM info
      # cat $job_script | grep MEM | cut -f 2,3,4 > $dat_name
      cat $job_script | grep -e "MEM[^I]" > $dat_name
    fi

  done
done

exit 0