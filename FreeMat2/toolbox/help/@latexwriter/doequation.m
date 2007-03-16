function doequation(&p,eqn)
  fprintf(p.myfile,'\\[\n%s\\]\n',strrep(eqn,'\','\\'));
  
  