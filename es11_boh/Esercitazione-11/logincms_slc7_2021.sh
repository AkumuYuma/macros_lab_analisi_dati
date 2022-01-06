#!/bin/bash

export SCRAM_ARCH=slc7_amd64_gcc900
#== ROOT ==
################-- old
#################source /cvmfs/cms.cern.ch/slc6_amd64_gcc630/lcg/root/6.12.07/bin/thisroot.sh
################-- very old
#################source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.32.04/x86_64-slc5-gcc43-opt/root/bin/thisroot.sh
#-- new
source /cvmfs/cms.cern.ch/slc7_amd64_gcc900/lcg/root/6.22.08-llifpc5/bin/thisroot.sh
#
#
##################################
#PYTHON (needed for sPlot exercise)
##alias python="/cvmfs/cms.cern.ch/slc6_amd64_gcc530/cms/CMSSW_9_0_0_pre4/external/slc6_amd64_gcc530/bin/python2.7"
##################################
#
#== GCC ==
################--old
################source /cvmfs/cms.cern.ch/slc5_amd64_gcc481/external/gcc/4.8.1/etc/profile.d/init.sh
#-- new
source /cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/gcc/9.3.0/etc/profile.d/init.sh
#
##############################################################################
#
################export PATH=$PATH:/afs/cern.ch/cms/slc5_amd64_gcc481/external/gcc/4.8.1/bin/
#
export PATH=$PATH:$ROOTSYS/bin:.
#
#################export CPATH=$CPATH:/cvmfs/cms.cern.ch/slc5_amd64_gcc481/lcg/roofit/5.34.18/include/
export CPATH=$CPATH:/cvmfs/cms.cern.ch/slc7_amd64_gcc900/lcg/root/6.22.08-llifpc5/include/
#
#== LIBs ==
#
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib:.
#
#################export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/cvmfs/cms.cern.ch/slc5_amd64_gcc481/external/gcc/4.8.1/lib
#################export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/cvmfs/cms.cern.ch/slc5_amd64_gcc481/external/gcc/4.8.1/lib64
#################export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/cvmfs/cms.cern.ch/slc5_amd64_gcc481/cms/cmssw/CMSSW_7_1_0/external/slc5_amd64_gcc481/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/gcc/9.3.0/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/cvmfs/cms.cern.ch/slc7_amd64_gcc900/external/gcc/9.3.0/lib64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/cvmfs/cms.cern.ch/slc7_amd64_gcc900/cms/cmssw/CMSSW_12_0_1/external/slc7_amd64_gcc900/lib