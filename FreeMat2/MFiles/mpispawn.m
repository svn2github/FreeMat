mpiinit
childcom = mpicommspawn('/usr/X11R6/bin/xterm',{'-e','/home/basu/FreeMat2/src/x11/FreeMat','-f','mpiserver'});
newcomm = mpiintercommmerge(childcom,0);
