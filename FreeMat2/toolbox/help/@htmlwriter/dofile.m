function dofile(&p,filename,textstring)
  fprintf(p.myfile,'<P>\n<PRE>\n');
  fprintf(p.myfile,'     %s\n',p.filename);
  fprintf(p.myfile,'%s\n',latin_filter(p.textstring));
  fprintf(p.myfile,'</PRE>\n');
  fprintf(p.myfile,'<P>\n');
