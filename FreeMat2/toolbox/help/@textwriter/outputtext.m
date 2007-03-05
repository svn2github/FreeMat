function outputtext(&p,text)
  if (p.ignore) return; end
  fprintf(p.myfile,'%s',expand_codes(text));
  