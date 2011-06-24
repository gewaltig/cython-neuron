
import numpy as np
import scipy as sp
import pylab as pl

T_sim = 1000.

datapath = 'data/'

mem_limit = 14294.5

##################################
# Data from Blue Gene (Tobias)
##################################

vps_s550 = np.array([ 4096,  8192, 16384, 32768])

# (d.h. exakt ist N=6.1875*106, K/N=11250)
st_s550_nosd = np.array([334.7227783203121, 206.14506835937405, 139.53292663574103, 108.56503112793136])

vps_s100 = np.array([ 1024,  2048,  4096,  8192, 16384])

# (d.h. exakt ist N=1.125*106, K/N=11250)
st_s100_nosd = np.array([195.45021484375002, 114.33687500000008, 70.624736328125095, 50.796391601562469, 49.783812255859395])

##################################
# Data from Kei
##################################


#N_procs = np.array([8, 16, 32, 64])
N_Nodes = [2**k for k in range(8, 11)] + [1536]
n_threads_per_proc = 8
N_procs = [N * n_threads_per_proc for N in N_Nodes]

# plotting style
markersize = 20.

# logfile structure
#0 0 75944 # hard_mem_0
#1 0 8230288 # virt_mem_0
#2 0 145984 # hard_mem_after_nodes 
#3 0 8230288 # virt_mem_after_nodes
#4 0 1.26 # build_time_nodes
#5 0 337.27 # build_edge_time 
#6 0 1252016 # hard_mem_after_edges
#7 0 8230288 # virt_mem_after_edges
#8 0 1281032 # hard_mem_after_presim
#9 0 8230288 # virt_mem_after_presim
#10 0 188.84 # presim_time
#11 0 1281048 # hard_mem_after_sim
#12 0 8230288 # virt_mem_after_sim
#13 0 63.29 # sim_time


def get_data(N_procs, filebase, n_files_try):

  runtime_sim = []
  runtime_setup = []
  n_procs = []
  mem = []

  for i, N_proc in enumerate(N_procs):

    base_dir = filebase + str(N_proc)
    print "loading ", base_dir
    rt_sim_total = 0.
    rt_setup_total = 0.
    mem_tot = 0.
    n_files = 0
    for p in xrange(n_files_try):
      runtime_name = base_dir + '/logfile_' + str(p) + '.dat'
      try:
        rt = pl.loadtxt(runtime_name)
        rt_setup_total += rt[4,2] + rt[5,2]
        rt_sim_total += rt[13,2]
        mem_tot += rt[11, 2]
        n_files += 1
      except:
        print "ignoring file ", runtime_name

    # was there at least one file?
    if n_files > 0:
      runtime_sim.append( rt_sim_total / n_files )
      runtime_setup.append( rt_setup_total / n_files )
      mem.append( mem_tot / n_files )
      n_procs.append(N_proc)

  return np.array(n_procs), np.array(runtime_sim), np.array(runtime_setup), np.array(mem)


threaded_N, threaded_sim, threaded_setup, threaded_mem = get_data(N_procs, datapath + 'sim_openmp_10k_N', 30)
flat_N, flat_sim, flat_setup, flat_mem =  get_data(N_procs, datapath + 'sim_flatmpi_10k_N', 30)

print threaded_sim, threaded_mem
print flat_sim, flat_mem


pl.figure(1)
pl.clf()
pl.title(r'NEST 10k : stdp_bm.sli : $N = 10^6, K = 10^4, T = 1 s$')
pl.loglog(threaded_N, threaded_sim, '.', markersize=markersize, label='Kei 8 threads hybrid')
pl.loglog(flat_N, flat_sim, '.', markersize=markersize, label='Kei flat MPI')
pl.loglog(vps_s100, st_s100_nosd, '.', markersize=markersize, label='Blue Gene')
pl.ylabel(r'simulation time (s)')
pl.xlabel(r'number of cores')
pl.legend()
pl.xticks([1024, 2048, 4096, 8192, 12288, 16384], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$12288$', r'$16384$'])
pl.yticks([50, 100, 200], [r'$50$', r'$100$', r'$200$'])
pl.xlim([512, 32768])
pl.ylim([30, 250])
pl.savefig('sim_K_stdp_bm_10k.eps')
pl.savefig('sim_K_stdp_bm_10k.pdf')


pl.figure(2)
pl.clf()
pl.title(r'NEST 10k : stdp_bm.sli : $N = 10^6, K = 10^4, T = 1 s$')
pl.loglog(threaded_N, threaded_setup/60., '.', markersize=markersize, label='8 threads hybrid')
pl.loglog(flat_N, flat_setup/60., '.', markersize=markersize, label='flat MPI')
pl.loglog([512, 32768], [60, 60], 'k--', label='current time lmit')

pl.xticks([1024, 2048, 4096, 8192, 12288, 16384], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$12288$', r'$16384$'])
pl.yticks([5., 10., 20., 40., 80], [r'$5$', r'$10$', r'$20$', r'$40$', r'$80$'])
pl.xlim([512, 32768])
pl.xlabel(r'number of cores')
pl.ylabel(r'setup time (min)')
pl.legend()

pl.savefig('setup_time_K_stdp_bm_10k.eps')
pl.savefig('setup_time_K_stdp_bm_10k.pdf')

pl.figure(3)
pl.clf()
pl.title(r'NEST 10k : stdp_bm.sli : $N = 10^6, K = 10^4, T = 1 s$')
pl.loglog(threaded_N, threaded_mem, '.', markersize=markersize, label='8 threads hybrid')
pl.loglog(flat_N, flat_mem, '.', markersize=markersize, label='flat MPI')
pl.loglog([512, 32768], [mem_limit*1024**2, mem_limit*1024**2], 'k--', label='current memory lmit')

pl.xticks([1024, 2048, 4096, 8192, 12288, 16384], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$12288$', r'$16384$'])
pl.yticks([5., 10., 20., 40., 80], [r'$5$', r'$10$', r'$20$', r'$40$', r'$80$'])
pl.xlim([512, 32768])
pl.xlabel(r'number of cores')
pl.ylabel(r'memory use per job (kB)')
pl.legend()

pl.savefig('setup_time_K_stdp_bm_10k.eps')
pl.savefig('setup_time_K_stdp_bm_10k.pdf')



pl.show()

