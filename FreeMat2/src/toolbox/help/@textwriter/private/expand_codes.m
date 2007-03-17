function text = expand_codes(text)
  text = regexprep(text,'\@\|([^\|]*)\|','$1');
  text = strrep(text,'\','\\');