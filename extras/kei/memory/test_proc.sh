#!/bin/bash
#!/bin/bash -x
#PJM --rsc-list "node=2"
#PJM --rsc-list "elapse=00:10:00"
#PJM --mpi "proc=2"
#PJM -s
. /work/system/Env_base

cat /proc/$$/statm

cat /proc/$$/status | grep 'Vm*'

echo 'meminfo_thisjob ==
memory_thisjob ==
hardware_memory_thisjob == ' > meminfo.sli

time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB /work/user0049/10kproject_install/bin/nest  meminfo.sli

#time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB  meminfo.sh
