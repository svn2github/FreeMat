function doitemize(&p,enums)
  for i = 1:numel(p.clients)
    doitemize(p.clients{i},enums);
  end
