function text = expand_codes(text)
  text = regexprep(text,'\@\|([^\|]*)\|','<code>$1</code>');
  if (strcmp(text,'\n')) 
     text = '<P>\n';
  end
