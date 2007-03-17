function endverbatim(&p)
  fprintf(p.myfile,'\\end{verbatim}\n');
  p.verbatim = false;
  