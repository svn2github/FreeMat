function doenumerate(&p,enums)
  for i = 1:numel(p.clients)
    doenumerate(&p.clients{i},enums);
  end
