function x = testeq(a,b)
  d = full(a)-full(b);
  if (strcmp(typeof(d),'double') | strcmp(typeof(d),'dcomplex'))
    x = isempty(find(abs(d)>eps));
  else
    x = isempty(find(abs(d)>feps));
  end
