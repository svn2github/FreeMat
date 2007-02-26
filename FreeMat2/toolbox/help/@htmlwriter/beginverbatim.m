function p = beginverbatim(p,text)
  fprintf(p.myfile,'<PRE>\n');
  p.verbatim = true;
