function endverbatim(&p)
  fprintf(p.myfile,'</PRE>\n<P>\n');
  p.verbatim = false;
  