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
%In the final form, the path command prints out the current path
%@[
%  path
%@]
%!

% Copyright (c) 2002-2006 Samit Basu

function x = path(a,b)
if (strcmp(computer,'PCWIN'))
  pathdiv = ';';
else
  pathdiv = ':';
end
if ((nargout == 0) && (nargin == 0))
    a = getpath;
    b = strfind(a,pathsep);
    n = 1;
    for i=1:numel(b)
        printf('%s\n',a(n:(b(i)-1)));
        n = b(i)+1;
    end
    printf('%s\n',a((b(end)+1):end));
    return;
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
