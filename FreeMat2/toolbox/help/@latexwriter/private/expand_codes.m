function text = expand_codes(text)
  text = regexprep(text,'\@\|([^\|]*)\|','\\verb|$1|');
  text = strrep(text,'\n','\\n');
  text = strrep(text,'\r','\\r');
  text = strrep(text,'\t','\\t');
  
  
  