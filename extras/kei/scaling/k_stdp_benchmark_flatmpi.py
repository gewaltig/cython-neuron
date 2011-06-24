
import os
import glob

# needs to be adapted
nest_install = '/work/user0049/10kproject_install/bin/'
sli_script = 'stdp_bm.sli'

# duration of simulation
T_sim = 1000.

# size of network
scale = 550


#N_Nodes = [2**k for k in range(7, 11)] + [1536]
N_Nodes = [2**k for k in range(9, 11)] + [1536]
#N_Nodes = [2**k for k in range(1,2)]


# scale = 100
# N_Nodes = 1024 memory error, MEMORY SIZE (USE)         : 14294.5 MiB (14988787712)
# N_Nodes = 2048 memory error, MEMORY SIZE (USE)         : 14294.5 MiB (14988828672)
# N_Nodes = 4096 T_elapse = 10:26 minutes, MEMORY SIZE (USE)         : 11146.9 MiB (11688280064)
# N_Nodes = 8192 T_elapse =8:11 minutes, MEMORY SIZE (USE)         : 7670.8 MiB (8043405312)

#expected_times = [30, 15, 20, 10, 10] # min
# for scale = 100
# expected_times = [28, 14, 10, 10] # min

# for scale = 550
expected_times = [60, 60, 60, 60] # min


cores_per_node = 8

N_procs = [N * cores_per_node for N in N_Nodes]

for i, N_proc in enumerate(N_procs):

       base_dir = 'sim_flatmpi_10k_6_N' + str(N_proc)
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
       f.write(str(scale) + ' /scale Set\n')
       f.write('(file) /recto Set\n')
       f.write('(.) /path_name Set\n')
       f.write('false /rtf Set\n')         # do not record spikes
       f.write('(logfile) /log_file Set\n')
       f.close()
 
       # copy simulation script to location
       os.system('cp *.sli ' + base_dir)

       # write a batch script and submit it

       f = open(base_dir + '/' + batch_name, 'w')
       f.write('#!/bin/bash -x\n')
       f.write('#PJM --rsc-list "node='+str(N_Nodes[i])+'"\n')
       f.write('#PJM --rsc-list "elapse=00:'+str(expected_times[i])+':00"\n') # maximum time
       f.write('#PJM --mpi "proc='+str(N_proc)+'"\n')
       f.write('#PJM -s\n')
       f.write('. /work/system/Env_base\n')

       f.write('time mpiexec lpgparm -s 4MB -d 4MB -h 4MB -t 4MB -p 4MB ' + nest_install + '/nest ' + sli_script + '\n')

       f.close()

       os.system('cd ' + base_dir + '; pjsub ' + batch_name)

