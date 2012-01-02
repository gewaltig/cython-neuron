
import os
import glob

# select version of NEST
# TODO #1: adapt dir of NEST install
nest_install = '/homea/jinb33/jinb3307/10kproject.build/bin/'
sli_script = 'stdp_bm_fast_wiring.sli'
absbasedir = os.getcwd()


# duration of simulation
T_sim = 1000.

# TODO #2: adapt this array for the number of nodes to use
N_Nodes = [512]

email = 'm.helias@fz-juelich.de'

threads_per_node = 4

# must be at least 10 minutes, maximum 60
# TODO #3: adapt wallclock time
expected_times = [60, 60, 60, 60, 60, 60] # min

# size of network
scale = 300
base_name = 'sim_openmp_3e6_N'

N_procs = [N * threads_per_node for N in N_Nodes]

for i, N_proc in enumerate(N_procs):

       base_dir = base_name + str(N_proc)
       batch_name = 'stdp_scale_'+str(N_proc)+'.ll'

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

       f.write('# @job_type         = bluegene\n')
       f.write('# @bg_size          = ' + str(N_Nodes[i]) + '\n')
       f.write('# @bg_connection    = TORUS\n')
       f.write('# @environment      = COPY_ALL\n')
       f.write('# @wall_clock_limit = 00:'+str(expected_times[i])+':00\n')
       f.write('# @job_name         = jugene_scaling_nest' + str(N_proc) + '\n')       
       f.write('# @output           = ' + absbasedir + '/' + base_dir + '/$(job_name).$(jobid).out\n')
       f.write('# @error            = ' + absbasedir + '/' + base_dir + '/$(job_name).$(jobid).err\n')

       f.write('# @notification     = error\n')
       f.write('# @notify_user      = ' + email + '\n')
       f.write('# @queue\n')

       f.write('mpirun -exe ' + nest_install + 'nest -np 512 -mode SMP -verbose 1 -args "' + absbasedir + '/' + base_dir + '/' + sli_script + '"\n')


       f.close()

       os.system('cd ' + base_dir + '; llsubmit ' + batch_name)


