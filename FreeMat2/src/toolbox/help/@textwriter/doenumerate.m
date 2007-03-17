function doenumerate(&p,itemlist)
  if (p.ignore) return; end;
  for i=1:numel(itemlist)
    fprintf(p.myfile,'  %d. %s\n',i,expand_codes(itemlist{i}));
  end
