# this script takes one (or more) output files of benchmark
# simulations as input and then
# - creates an equivalent directory in $HOME/data
# - extracts the different times from the std-out file into temporary files
# - loads these into python and pickles the data
# - potential simulation output gets targzed
# - copies all relevant data to the $HOME/data structure
# The resulting $HOME/data structure can be scpied for visualization.
#
# Arguments:
# full paths to files containing output of (single run) benchmark 
#
# Tobias Potjans, 22nd June 2010
numpy_available=False

import sys
import os
import pickle
if numpy_available:
    import numpy

# JUGENE settings
homeprefix='/homea/jinb33/jinb3302/data/'
workprefix='/work/jinb33/jinb3302/data/'
# testing cnpsn-tcp settings
#homeprefix='/home/tpotjans/tmp/data/'
#workprefix='/home/tpotjans/cnp/data/'
picklefname='bm_data.pck'

for workfile in sys.argv[1:]:
    # 1. copy directory substructure from work to home
    filename = workfile.split('/')[-1]
    reldir = workfile.split(workprefix)[1].split(filename)[0]
    workdir = workprefix+reldir
    homedir = homeprefix+reldir
    os.system('mkdir -p '+homeprefix+reldir)

    # 2. create in work the files containing only info on times and memory
    #    - BuildNodeTime
    #    - BuildEdgeTime
    #    - PreparationTime
    #    - SimCPUTime
    #    - TotalTime
    #    - NodeMem
    #    - NodeConnMem
    #    - NodeConnPrepsimMem
    os.system('cat '+workfile+' | grep "] BuildNode" | cut -d " " -f 9 > '+homedir+'bntime.dat')
    os.system('cat '+workfile+' | grep "] BuildEdge" | cut -d " " -f 9 > '+homedir+'betime.dat')
    os.system('cat '+workfile+' | grep "] Preparation" | cut -d " " -f 7 > '+homedir+'preptime.dat')
    os.system('cat '+workfile+' | grep "] Simulation time" | cut -d " " -f 7 > '+homedir+'simtime.dat')
    os.system('cat '+workfile+' | grep "] Total" | cut -d " " -f 7 > '+homedir+'totaltime.dat')
    os.system('cat '+workfile+' | grep NodeMem | cut -d " " -f5 | cut -d ")" -f 1 > '+homedir+'nodemem.dat')
    # correct version
    os.system('cat '+workfile+' | grep NodeConnMem | cut -d " " -f5 | cut -d ")" -f 1 > '+homedir+'nodeconnmem.dat')
    os.system('cat '+workfile+' | grep NodeConnPrepsimMem | cut -d " " -f5 | cut -d ")" -f 1 > '+homedir+'nodeconnprepsimmem.dat')
    # old version for some older benchmarks
    #os.system('cat '+workfile+' | grep BGPMem | cut -d " " -f5 | cut -d ")" -f 1 > '+homedir+'nodeconnmem.dat')
    
    # 3. load these files and jointly pickle this data in home
    if numpy_available:
        bnt = numpy.loadtxt(workdir+'bntime.dat')
        bet = numpy.loadtxt(workdir+'betime.dat')
        pt  = numpy.loadtxt(workdir+'preptime.dat')
        st  = numpy.loadtxt(workdir+'simtime.dat')
        tt  = numpy.loadtxt(workdir+'totaltime.dat')
        n_mem  = numpy.loadtxt(workdir+'nodemem.dat')
        nc_mem = numpy.loadtxt(workdir+'nodeconnmem.dat')
        #ncp_mem = numpy.loadtxt(workdir+'nodeconnprepsimmem.dat')
        pfile = open(homedir+picklefname,'w')
        pickle.dump([bnt,bet,pt,st,tt,n_mem,nc_mem],pfile)
        pfile.close()   

    # 4. tar additional data (scripts for reproducibility and data) in home
    backupfilestring = filename+' data *.txt *.sli *.ll *.sh'
    currentdir = os.getcwd()
    os.chdir(workdir)
    os.system('tar czf backupfiles.tgz '+backupfilestring)
    os.system('mv backupfiles.tgz '+homedir)
    os.chdir(currentdir)
    
    



