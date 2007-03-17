function endverbatim(&p)
  for i=1:numel(p.clients)
    endverbatim(p.clients{i});
  end
