function beginverbatim(&p,text)
  fprintf(p.myfile,'\\begin{verbatim}\n');
  p.verbatim = true;
  