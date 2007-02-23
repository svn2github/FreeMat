function beginmodule(&p,sourcepath,modname,moddesc,secname)
  for i = 1:numel(p.clients)
    beginmodule(p.clients{i},sourcepath,modname,moddesc,secname);
  end