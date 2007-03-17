function beginverbatim(&p,text)
  if (p.ignore) return; end
  p.verbatim = true;
  fprintf(p.myfile,'\n');
