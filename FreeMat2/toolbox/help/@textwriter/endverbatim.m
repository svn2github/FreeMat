function endverbatim(&p)
  fprintf(p.myfile,'\n');
  p.verbatim = false;
  