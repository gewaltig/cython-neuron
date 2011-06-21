
import os
import glob

# needs to be adapted
nest_install = '/work/user0049/nest2_openmp_install/bin/'
sli_script = 'stdp_bm.sli'

T_sim = 1000.


N_Nodes = [2**k for k in range(7, 11)] + [1536]
#N_Nodes = [2**k for k in range(1,2)]

expected_time = 60 # min

cores_per_node = 8

N_procs = [N * cores_per_node for N in N_Nodes]

for i, N_proc in enumerate(N_procs):

       base_dir = 'sim_flatmpi_6_10_6_N' + str(N_proc)
       batch_name = 'stdp_scale_'+str(N_proc)+'.sh'

       os.system('mkdir -p ' + base_dir)
       os.system('rm -f ' + base_dir + '/' + batch_name + '*')

       # write a param conf file for the simulation
       # % parameters defined in param.conf.sli:
       # % nvp - number of virtual processes
       # % Tsim - total simulation time in ms
       # % recto - argument for /record_to variable of spike detectors
       # % path_name - path where all files will have to be written
       # % the file is usually created by a bash script
       f = open(base_dir + '/param.conf.sli', 'w')
       f.write(str(N_proc) + ' /nvp Set\n')
       f.write(str(T_sim) + ' /Tsim Set\n')
       f.write('(file) /recto Set\n')
       f.write('(.) /path_name Set\n')
       f.write('false /rtf Set\n')         # do not record spikes
       f.write('(runtimes) /time_file Set\n')
       f.close()
 
       # copy simulation script to location
       os.system('cp ' + sli_script + ' ' + base_dir)

       # write a batch script and submit it

       f = open(base_dir + '/' + batch_name, 'w')
       f.write('#!/bin/bash -x\n')
       f.write('#PJM --rsc-list "node='+str(N_Nodes[i])+'"\n')
       f.write('#PJM --rsc-list "elapse=00:'+str(expected_time)+':00"\n') # maximum time
       f.write('#PJM --mpi "proc='+str(N_proc)+'"\n')
       f.write('#PJM -s\n')
       f.write('. /work/system/Env_base\n')

       f.write('time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB ' + nest_install + '/nest ' + sli_script + '\n')

       f.close()

       os.system('cd ' + base_dir + '; pjsub ' + batch_name)

