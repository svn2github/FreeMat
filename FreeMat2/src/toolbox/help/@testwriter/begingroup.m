function begingroup(&p,groupname)
  p.groupname = groupname;
  if (~strcmp(lower(groupname),'tests'))
    p.ignore = 1;
  else
    p.ignore = 0;
  end
