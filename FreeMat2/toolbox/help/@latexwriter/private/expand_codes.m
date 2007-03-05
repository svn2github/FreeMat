function text = expand_codes(text)
  text = regexprep(text,'\@\|([^\|]*)\|','\\verb|$1|');
  