function doenumerate(&p,itemlist)
  fprintf(p.myfile,'\\begin{enumerate}\n');
  for i=1:numel(itemlist)
    fprintf(p.myfile,'\\item %s\n',itemlist{i});
  end
  fprintf(p.myfile,'\\end{enumerate}\n');
  