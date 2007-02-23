function beginverbatim(&p)
  for i=1:numel(p.clients)
    beginverbatim(p.clients{i});
  end
