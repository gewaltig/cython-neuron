#!/bin/bash

datapath="./data/"

nest="/work/user0049/10kproject_install/bin/nest"
sli_script="get_mem_params.sli"

#rm -rf $datapath
mkdir -p $datapath
cp $sli_script $datapath
cd $datapath

# more than 10 minutes for
#M96_N98304_K4096

#M192_N98304_K8192
#M384_N98304_K8192
#M768_N98304_K8192
#M1536_N98304_K8192
#M3072_N98304_K8192
#M6144_N98304_K8192
#M12288_N98304_K8192

# more than 30 minutes for
#M96_N98304_K8192

M=96
N=98304
K=8192

N_NODES=$(($M/8))

  
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
#PJM --rsc-list \"elapse=00:60:00\"
#PJM --mpi \"proc="$M"\"
#PJM -s
. /work/system/Env_base
time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB "$nest" "$config" "$sli_script"
" > $job_script

  # submit the job
pjsub $job_script

exit 0

for M in 96 192 384 768 1536 3072 6144 12288
do
  # determine number of nodes required (8 cores/node)
  N_NODES=$(($M/8))

  N=98304
  K=8192
  
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
#PJM --rsc-list \"elapse=00:30:00\"
#PJM --mpi \"proc="$M"\"
#PJM -s
. /work/system/Env_base
time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB "$nest" "$config" "$sli_script"
" > $job_script

  # submit the job
  pjsub $job_script

done
