
mpiinit
% Enter a process loop
quitflag = 0;
while (1) 
  % Read the string to execute
  toexec = mpirecv(0,2840);
  eval(toexec);
end
