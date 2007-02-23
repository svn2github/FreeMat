function dofile(&p,filename,text)
  for i = 1:numel(p.clients)
    dofile(p.clients{i},filename,text);
  end
