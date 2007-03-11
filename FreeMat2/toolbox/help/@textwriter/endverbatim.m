function endverbatim(&p)
  if (p.ignore); return; end;
  fprintf(p.myfile,'\n');
  p.verbatim = false;
  