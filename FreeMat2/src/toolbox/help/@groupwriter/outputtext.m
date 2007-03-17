function outputtext(&p,text)
  for i=1:numel(p.clients)
    outputtext(p.clients{i},text);
  end
