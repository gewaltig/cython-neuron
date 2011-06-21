
import numpy as np
import scipy as sp
import pylab as pl

T_sim = 1000.

datapath = 'data/'


#N_procs = np.array([8, 16, 32, 64])
N_Nodes = [2**k for k in range(8, 11)] + [1536]
n_threads_per_proc = 8
N_procs = [N * n_threads_per_proc for N in N_Nodes]

# plotting style
markersize = 20.

def get_threaded_data():

  runtime_sim = np.zeros(len(N_procs), dtype='float')
  runtime_setup = np.zeros(len(N_procs), dtype='float')

  for i, N_proc in enumerate(N_procs):

    base_dir = datapath + 'sim_openmp_N' + str(N_proc)
    print "loading ", base_dir
    rt_sim_total = 0.
    rt_setup_total = 0.
    n_files = 0
    for p in xrange(N_proc/n_threads_per_proc):
      runtime_name = base_dir + '/runtimes_' + str(p) + '.dat'
      try:
        rt = pl.loadtxt(runtime_name)
        rt_setup_total += rt[0,2] #+ rt[2,2]
        rt_sim_total += rt[1,2]
        n_files += 1
      except:
        print "ignoring file ", runtime_name

       
    runtime_sim[i] = rt_sim_total / n_files
    runtime_setup[i] = rt_setup_total / n_files

  return runtime_sim, runtime_setup



def get_flat_mpi_data():

  runtime_sim = np.zeros(len(N_procs), dtype='float')
  runtime_setup = np.zeros(len(N_procs), dtype='float')

  for i, N_proc in enumerate(N_procs):

    base_dir = datapath + 'sim_flatmpi_N' + str(N_proc)
    print "loading ", base_dir
    rt_sim_total = 0.
    rt_setup_total = 0.
    n_files = 0
    for p in xrange(N_proc):
      runtime_name = base_dir + '/runtimes_' + str(p) + '.dat'
      try:
        rt = pl.loadtxt(runtime_name)
        rt_setup_total += rt[0,2] #+ rt[2,2]
        rt_sim_total += rt[1,2]
        n_files += 1
      except:
        print "ignoring file ", runtime_name


      rt = pl.loadtxt(runtime_name)
      rt_setup_total += rt[0,2] #+ rt[2,2]
      rt_sim_total += rt[1,2]
       
    runtime_sim[i] = rt_sim_total / n_files
    runtime_setup[i] = rt_setup_total / n_files

  return runtime_sim, runtime_setup


threaded_sim, threaded_setup = get_threaded_data()
flat_sim, flat_setup = get_flat_mpi_data()

print threaded_sim
print flat_sim

pl.figure(1)
pl.clf()
pl.title(r'stdp_bm.sli on K: $N = 10^6, K = 10^4, T = 1 s$')
pl.loglog(N_procs, threaded_sim, '.', markersize=markersize, label='hybrid')
pl.loglog(N_procs, flat_sim, '.', markersize=markersize, label='flat MPI')
pl.ylabel(r'simulation time')
pl.xlabel(r'number of cores')
pl.legend()
pl.savefig('sim_K_stdp_bm.eps')
pl.savefig('sim_K_stdp_bm.pdf')

pl.figure(2)
pl.clf()
pl.title(r'stdp_bm.sli on K: $N = 10^6, K = 10^4, T = 1 s$')
pl.semilogx(N_procs, flat_sim/threaded_sim, '.', markersize=markersize)
pl.ylabel(r'gain hybrid/flat MPI')
pl.xlabel(r'number of cores')
pl.legend()
pl.savefig('gain_K_stdp_bm.eps')
pl.savefig('gain_K_stdp_bm.pdf')

pl.figure(3)
pl.clf()
pl.title('setup time')
pl.loglog(N_procs, threaded_setup, '.', markersize=markersize, label='hybrid')
pl.loglog(N_procs, flat_setup, '.', markersize=markersize, label='flat MPI')
pl.ylabel(r'setup time')
pl.xlabel(r'number of cores')
pl.legend()

pl.show()

