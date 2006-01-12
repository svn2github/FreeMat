%!
%@Module MPIRUN MPI Process Run
%@@Section MPI
%@@Usage
%This function is a simple example of how to use FreeMat and MPI
%to execute functions remotely.  More documentation on how to
%use this function will be written later...
%!
function mpirun(toexec,ranks)
  for rank=ranks
     mpisend(toexec,rank,2840);
  end

