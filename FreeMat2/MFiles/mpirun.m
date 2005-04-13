
function mpirun(toexec,ranks)
  for rank=ranks
     mpisend(toexec,rank,2840);
  end

