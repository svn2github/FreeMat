function newcomm = mpispawn(count,xterm)
  if (~exist('count'))
    count = 1;
  end
  if (~exist('xterm'))
    xterm = 0;
  end
  if (xterm)
    childcom = mpicommspawn('xterm',{'-e','FreeMat','-f','mpiserver'},count);
  else
    childcom = mpicommspawn('FreeMat',{'-f','mpiserver'},count);
  end
  newcomm = mpiintercommmerge(childcom,0);
