function doenumerate(&p,itemlist)
  for i=1:numel(itemlist)
    fprintf(p.myfile,'  %d. %s\n',i,expand_codes(itemlist{i}));
  end
