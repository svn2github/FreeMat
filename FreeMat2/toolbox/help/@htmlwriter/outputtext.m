function outputtext(&p,text)
  text = latin_filter(text);
  if (p.verbatim)
    fprintf(p.myfile,'%s',text);
  else
    fprintf(p.myfile,'%s',expand_codes(text));
  end
  