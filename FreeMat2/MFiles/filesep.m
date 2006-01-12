%!
%@Module FILESEP Directory Separation Character
%@@Section FREEMAT
%@@Usage
%The @|filesep| routine returns the character used to separate directory
%names on the current platform (basically, a forward slash for Windows,
%and a backward slash for all other OSes).  The syntax is simple:
%@[
%  x = filesep
%@]
%!
function x = filesep
if (strcmp(computer,'PCWIN'))
  x = '\';
else
  x = '/';
end
