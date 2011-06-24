
import numpy as np
import scipy as sp
import pylab as pl

T_sim = 1000.

datapath = 'data/'


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
markersize = 10.

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


def get_data_old(N_procs, filedir, filestem, n_files_try):

  runtime_sim = []
  runtime_setup = []
  n_procs = []
  mem = []

  for i, N_proc in enumerate(N_procs):

    basedir = filedir + str(N_proc)
    print "loading ", basedir
    rt_sim_total = 0.
    rt_setup_total = 0.
    mem_tot = 0.
    n_files = 0
    for p in xrange(n_files_try):
      runtime_name = basedir + '/' + filestem + str(p) + '.dat'
      try:
        rt = pl.loadtxt(runtime_name)
        rt_setup_total += rt[0,2]
        rt_sim_total += rt[1,2]
        n_files += 1
      except:
        print "ignoring file ", runtime_name

    # was there at least one file?
    if n_files > 0:
      runtime_sim.append( rt_sim_total / n_files )
      runtime_setup.append( rt_setup_total / n_files )
      mem.append( mem_tot / n_files )
      n_procs.append(N_proc)

  return n_procs, runtime_sim, runtime_setup, mem


def get_data(N_procs, filedir, filestem, n_files_try):

  runtime_sim = []
  runtime_setup = []
  n_procs = []
  mem = []

  for i, N_proc in enumerate(N_procs):

    basedir = filedir + str(N_proc)
    print "loading ", basedir
    rt_sim_total = 0.
    rt_setup_total = 0.
    mem_tot = 0.
    n_files = 0
    for p in xrange(n_files_try):
      runtime_name = basedir + '/' + filestem + str(p) + '.dat'
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

  return n_procs, runtime_sim, runtime_setup, mem




# simulated in /work/user0049/tobias_benchmark_ohno
threaded_n, threaded_sim, threaded_setup, threaded_mem = get_data_old(N_procs, 'data/sim_openmp_N', 'runtimes_', 30)

# simulated in /work/user0049/tobias_benchmark
threaded_new_n, threaded_new_sim, threaded_new_setup, threaded_new_mem = get_data(N_procs, 'data/sim_openmp_N', 'logfile_', 30)

# N=2 10^6
threaded_n_2, threaded_sim_2, threaded_setup_2, threaded_mem_2 = get_data(N_procs, 'data/sim_openmp_2_N', 'logfile_', 30)

print threaded_n_2
print threaded_sim_2
print threaded_setup_2
print threaded_mem_2
#[4096, 8192, 12288]
#[105.4293333333333, 78.07999999999997, 73.721999999999994]
#[2532.2903333333334, 1298.6936666666666, 878.83966666666686]
#[14191202.4, 9980271.4666666668, 7639882.9333333336]


threaded_all_n = threaded_n + threaded_new_n
threaded_all_sim = threaded_sim + threaded_new_sim
threaded_all_setup = threaded_setup + threaded_new_setup
threaded_all_mem = threaded_mem + threaded_new_mem

print threaded_all_n
print threaded_all_sim
print threaded_all_setup
print threaded_all_mem

# results in
# threaded_n = [2048, 4096, 8192, 12288, 10000]
# threaded_sim = [75.806999999999974, 49.205666666666694, 41.579666666666661, 101.90300000000001, 48.616666666666639]
# threaded_setup = [1.6413333333333333, 1.637333333333334, 1.6363333333333339, 1.639, 329.3893333333333]
# threaded_mem = [0.0, 0.0, 0.0, 0.0, 4427319.4666666668]


pl.figure(1)
pl.clf()
pl.title(r'NEST STDP benchmark, $K = 10^4$')


pl.loglog(threaded_all_n[:-1], threaded_all_sim[:-1], 'r.', markersize=markersize, label='Kei MPI + OpenMP(8), $N = 10^6$')

pl.loglog(threaded_n_2[:], threaded_sim_2[:], '.', color=mygreen, markersize=markersize, label=r'Kei MPI + OpenMP(8), $N=2\; 10^6$')

# Blue Gene 
pl.loglog(vps_s100[1:-1], st_s100_nosd[1:-1], 'b.', markersize=markersize, label='Jugene, $N = 10^6$')

T_m = threaded_all_sim[0]
T_n = threaded_all_sim[1]
m = threaded_n[0]
n = threaded_n[1]

pl.loglog([m, n], [T_m, T_n], 'k-', linewidth=2.)

pl.text(1100., 45., r'$\alpha_\mathrm{strong} = \frac{T_m/T_n}{n/m} = %2.2f$' % ( T_m/T_n / (n/m) ) )


#T_m = threaded_all_sim[0]
#T_n = threaded_all_sim[2]
#m = threaded_n[0]
#n = threaded_n[2]
#print m, n
#print T_m, T_n

#pl.loglog([m, n], [T_m, T_n], 'k-', linewidth=2.)

#pl.text(4200., 55., r'$\alpha_\mathrm{strong} = \frac{T_m/T_n}{n/m} = %2.2f$' % ( T_m/T_n / (n/m) ) )

T_m = threaded_sim_2[0]
T_n = threaded_sim_2[1]
m = threaded_n_2[0]
n = threaded_n_2[1]

pl.loglog([m, n], [T_m, T_n], 'k-', linewidth=2.)

pl.text(5000., 100., r'$\alpha_\mathrm{strong} = %2.2f$' % ( T_m/T_n / (n/m) ) )


pl.loglog([1024, 1024], [30., 250], 'k--')
pl.loglog([12288, 12288], [30., 250], 'k--')

pl.xticks([])
pl.yticks([])

pl.xlim([768, 16384])
pl.ylim([30, 250])
pl.ylabel(r'simulation time (s)')
pl.xlabel(r'number of cores')
pl.legend()

pl.xticks([1024, 2048, 4096, 8192, 12288], [r'$1024$', r'$2048$', r'$4096$', r'$8192$', r'$12288$'])
pl.yticks([50, 100, 200], [r'$50$', r'$100$', r'$200$'])

pl.savefig('sim_K_stdp_bm_final.eps')
pl.savefig('sim_K_stdp_bm_final.pdf')




pl.show()

