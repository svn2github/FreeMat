mpiinit
parent = mpicommgetparent;
newcomm = mpiintercommmerge(parent,1);
% Enter a process loop
quitflag = 0;
while (1) 
  % Read the string to execute
  toexec = mpirecv(0,2840,newcomm);
  eval(toexec);
end
