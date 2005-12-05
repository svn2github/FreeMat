
function x = path(a,b)
if (strcmp(computer,'PCWIN'))
  pathdiv = ';';
else
  pathdiv = ':';
end
if (nargout == 1)
  x = getpath;
else
  x = [];
end
if (nargin == 1)
  setpath(a);
elseif (nargin == 2)
  setpath([a,pathsep,b]);  
end
