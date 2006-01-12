%!
%@Module MPISERVER MPI Process Server
%@@Section MPI
%@@Usage
%This function is a simple example of how to use FreeMat and MPI
%to execute functions remotely.  More documentation on how to
%use this function will be written later...
%!
mpiinit
% Enter a process loop
quitflag = 0;
while (1) 
  % Read the string to execute
  toexec = mpirecv(0,2840);
  eval(toexec);
end
