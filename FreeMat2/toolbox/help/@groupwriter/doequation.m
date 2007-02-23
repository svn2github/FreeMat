function doequation(&p,eqn)
  for i = 1:numel(p.clients)
    doequation(&p.clients{i},eqn);
  end
