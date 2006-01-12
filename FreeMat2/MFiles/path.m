%!
%@Module PATH Get or Set FreeMat Path
%@@Section FREEMAT
%@@Usage
%The @|path| routine has one of the following syntaxes.  In the first form
%@[
%  x = path
%@]
%@|path| simply returns the current path.  In the second, the current path
%is replaced by the argument string @|'thepath'|
%@[
%  path('thepath')
%@]
%In the third form, a new path is appended to the current search path
%@[
%  path(path,'newpath')
%@]
%In the fourth form, a new path is prepended to the current search path
%@[
%  path('newpath',path)
%@]
%!
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
