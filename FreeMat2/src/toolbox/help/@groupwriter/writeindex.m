function writeindex(&p)
  for i = 1:numel(p.clients)
    writeindex(p.clients{i});
  end
