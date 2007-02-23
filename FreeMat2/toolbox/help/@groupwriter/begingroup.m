function begingroup(&p,groupname)
  for i = 1:numel(p.clients)
    begingroup(p.clients{i},groupname);
  end
