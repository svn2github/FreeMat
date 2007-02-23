function otext = latin_filter(text)
  otext = strrep(text,'&','&amp;');
  otext = strrep(otext,'<','&lt;');
  otext = strrep(otext,'>','&gt;');
