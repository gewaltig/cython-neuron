#!/bin/bash

job_dir=$WORK/"comm_time"
mkdir $job_dir

nest=$HOME/10k_project/gather_time_kei/10kproject_bld/bin/nest
sli_script_constNperM=$HOME/10k_project/gather_time_kei/get_nest_gather_time_constNperM.sli

for M in 2048 4096 8192 16384 32768 65536 131072
do
  job_name="comm_time_nu10.0_M"$M
  
  # create job file
  job_file=$job_dir/$job_name".ll"
  echo "# @job_type         = bluegene
# @bg_size          = $(( M/4 ))
# @bg_connection    = TORUS
# @environment      = COPY_ALL
# @wall_clock_limit = 02:00:00
# @job_name         = $job_name
# @output           = $job_dir/\$(job_name).\$(jobid).out
# @error            = $job_dir/\$(job_name).\$(jobid).err
# @notification     = error
# @notify_user      = kunkel@bcf.uni-freiburg.de
# @queue
" > $job_file

  for N_per_M in 1 2 4 8 16 32 64 128 256 512 1024
  do
    # create config file
    config=$job_dir/$job_name"_NperM"$N_per_M"_config.sli"
    echo "/M $M def
/N_per_M $N_per_M def
/nu 10.0 def
" > $config

    echo "mpirun -exe $nest -np $M -mode VN -verbose 1 -args \"$config $sli_script_constNperM\"
" >> $job_file

  done
done
