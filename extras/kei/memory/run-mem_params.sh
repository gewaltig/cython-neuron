#!/bin/bash

datapath="./data/"

nest="/work/user0049/10kproject_install/bin/nest"
sli_script="get_mem_params.sli"

rm -rf $datapath
mkdir -p $datapath
cp $sli_script $datapath
cd $datapath

for M in 96 192 384 768 1536 3072 6144 12288
do
  # determine number of nodes required (8 cores/node)
  N_NODES=$(($M/8))

  K=0
  for N in 24576 49152 98304 196608 393216 786432 1572864
  do
     # write a submission script for Kei queue
     job_name="mem_params_M"$M"_N"$N"_K"$K
     job_script=$job_name".sh"

     echo "#!/bin/bash -x
#PJM --rsc-list \"node="$N_NODES"\"
#PJM --rsc-list \"elapse=00:10:00\"
#PJM --mpi \"proc="$M"\"
#PJM -s
. /work/system/Env_base
time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB "$nest" "$config" "$sli_script > $job_script


      # create config file
      config=$job_name"_config.sli"
      echo "/M $M def
/N $N def
/K $K def
" > $config
    
      # submit the job
      pjsub $job_script
  done

  N=98304
  for K in 128 256 512 1024 2048 4096 8192
  do
      # create config file
      job_name="mem_params_M"$M"_N"$N"_K"$K
      job_script=$job_name".sh"

      config=$job_name"_config.sli"
      echo "/M $M def
/N $N def
/K $K def
" > $config

     # write a submission script for Kei queue
     echo "#!/bin/bash -x
#PJM --rsc-list \"node="$N_NODES"\"
#PJM --rsc-list \"elapse=00:10:00\"
#PJM --mpi \"proc="$M"\"
#PJM -s
. /work/system/Env_base
time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB "$nest" "$config" "$sli_script"
" > $job_script

     # submit the job
     pjsub $job_script

  done
done
