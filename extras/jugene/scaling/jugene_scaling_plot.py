
import numpy as np
import scipy as sp
import pylab as pl

T_sim = 1000.

datapath = './'
#basename = 'sim_openmp_3e6_N'
basename = 'sim_openmp_1e6_N'

# plotting style
markersize = 5.




pl.rcParams['font.size'] = 12
pl.rcParams['legend.fontsize'] = 10
pl.rcParams['figure.figsize'] = (5., 5.)
pl.rcParams['figure.dpi'] = 200
pl.rcParams['figure.subplot.left'] = 0.15
pl.rcParams['figure.subplot.right'] = 0.99
pl.rcParams['figure.subplot.top'] = 0.90
pl.rcParams['figure.subplot.bottom'] = 0.12
pl.rcParams['xtick.major.size'] = 8.      # major tick size in points
pl.rcParams['xtick.minor.size'] = 0.      # minor tick size in points
pl.rcParams['ytick.major.size'] = 8.      # major tick size in points
pl.rcParams['ytick.minor.size'] = 0.      # minor tick size in points


myblue = (0., 64./255., 192./255.)
myred = (192./255., 64./255., 0.)
mygreen = (0., 192./255., 64./255.)
myorange = (0.5, 0.25, 0.25)
mypink = (0.75, 0.25, 0.75)



##################################
# Old data from Jugene (Tobias)
##################################

vps_s100 = np.array([ 1024,  2048,  4096,  8192, 16384])

# (d.h. exakt ist N=1.125*106, K/N=11250)
st_s100_nosd = np.array([195.45021484375002, 114.33687500000008, 70.624736328125095, 50.796391601562469, 49.783812255859395])


##################################
# Data from Jugene
##################################



N_Nodes = [512, 1024, 2048, 4096]

n_threads_per_proc = 4
N_procs = [N * n_threads_per_proc for N in N_Nodes]

# plotting style
markersize = 20.


# structure of file is like this
#
#0 25 65144   # mem_0
#1 25 241496  # mem_after_nodes 
#2 25 2.61    # build_time_nodes
#3 25 765.19  # build_edge_time 
#4 25 7155744 # mem_after_edges
#5 25 7213424 # mem_after_presim
#6 25 82.1    # presim_time
#7 25 7213424 # mem_after_sim
#8 25 49.87   # sim_time

def get_data(N_procs, filedir, filestem, n_files_try):

  runtime_sim = []
  runtime_setup = []
  n_procs = []
  mem0 = []
  mem1 = []
  mem2 = []
  mem3 = []
  mem4 = []

  for i, N_proc in enumerate(N_procs):

    basedir = filedir + str(N_proc)
    print "loading ", basedir
    rt_sim_total = 0.
    rt_setup_total = 0.
    mem_tot0 = 0.
    mem_tot1 = 0.
    mem_tot2 = 0.
    mem_tot3 = 0.
    mem_tot4 = 0.
    n_files = 0
    for p in xrange(n_files_try):
      runtime_name = basedir + '/' + filestem + str(p) + '.dat'
      try:
        rt = pl.loadtxt(runtime_name)
        rt_setup_total += rt[2,2] + rt[3,2]
        rt_sim_total += rt[8,2]
        mem_tot0 += rt[0, 2]
        mem_tot1 += rt[1, 2]
        mem_tot2 += rt[4, 2]
        mem_tot3 += rt[5, 2]
        mem_tot4 += rt[7, 2]
        n_files += 1
      except:
        print "ignoring file ", runtime_name

    # was there at least one file?
    if n_files > 0:
      runtime_sim.append( rt_sim_total / n_files )
      runtime_setup.append( rt_setup_total / n_files )
      mem0.append( mem_tot0 / n_files )
      mem1.append( mem_tot1 / n_files )
      mem2.append( mem_tot2 / n_files )
      mem3.append( mem_tot3 / n_files )
      mem4.append( mem_tot4 / n_files )
      n_procs.append(N_proc)

  return n_procs, runtime_sim, runtime_setup, mem0, mem1, mem2, mem3, mem4


threaded_n,\
threaded_sim,\
threaded_setup,\
threaded_mem0,\
threaded_mem1,\
threaded_mem2,\
threaded_mem3,\
threaded_mem4 = get_data(N_procs, basename, 'logfile_', 30)


print threaded_n
print threaded_sim
print threaded_setup
print threaded_mem4


pl.figure(1)
pl.clf()
pl.title(r'NEST STDP benchmark, $K = 10^4$')

pl.loglog(vps_s100, st_s100_nosd, 'b.', markersize=markersize, label='old NEST trunk')

pl.loglog(threaded_n, threaded_sim, '.', markersize=markersize, color=myred, label='NEST 10k OpenMP(4)')


T_m = threaded_sim[1]
T_n = threaded_sim[2]
m = threaded_n[1]
n = threaded_n[2]


pl.loglog([m, n], [T_m, T_n], 'k-', linewidth=2.)

pl.text(6000., 90., r'$\alpha_\mathrm{strong} = \frac{T_m/T_n}{n/m} = %2.2f$' % ( T_m/T_n / (n/m) ) )


pl.xticks([])
pl.yticks([])

pl.ylabel(r'simulation time (s)')
pl.xlabel(r'number of cores')
pl.legend()

pl.xticks([1024, 2048, 4096, 8192, 16384], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$16384$'])
pl.yticks([25, 50, 100, 200, 400], [r'$25$', r'$50$', r'$100$', r'$200$', r'$400$'])

pl.xlim([800, 24000])
pl.ylim([25, 800])

pl.legend()
pl.savefig('sim_jugene_stdp.pdf')

#######################
## memory consumption

pl.rcParams['figure.subplot.left'] = 0.20

K = 1024.
pl.figure(2)
pl.clf()
pl.semilogx(threaded_n, threaded_mem0, '.', markersize=markersize, label='after start')
pl.semilogx(threaded_n, threaded_mem1, '.', markersize=markersize, label='after nodes')
pl.semilogx(threaded_n, threaded_mem2, '.', markersize=markersize, label='after edges')
pl.semilogx(threaded_n, threaded_mem3, '.', markersize=markersize, label='after presim')
pl.semilogx(threaded_n, threaded_mem4, '.', markersize=markersize, label='after sim')
pl.legend()

pl.xticks([1024, 2048, 4096, 8192, 16384], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$16384$'])


pl.savefig('memory_jugene_stdp.pdf')

pl.show()

