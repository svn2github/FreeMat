%!
%@Module PATHSEP Path Directories Separation Character
%@@Section FREEMAT
%@@Usage
%The @|pathsep| routine returns the character used to separate multiple directories
%on a path string for the current platform (basically, a semicolon for Windows,
%and a regular colon  for all other OSes).  The syntax is simple:
%@[
%  x = pathsep
%@]
%!
function x = pathsep
if (strcmp(computer,'PCWIN'))
  x = ';';
else
  x = ':';
end
