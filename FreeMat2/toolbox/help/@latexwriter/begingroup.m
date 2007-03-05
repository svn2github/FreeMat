function begingroup(&p,groupname)
  p.groupname = groupname;
  if (strcmp(lower(groupname),'tests'))
    p.ignore = 1;
  else
    fprintf(p.myfile,'\\subsection{%s}\n\n',p.groupname);
    p.ignore = 0;
  end
  