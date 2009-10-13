% PATHSEP PATHSEP Path Directories Separation Character
% 
% Usage
% 
% The pathsep routine returns the character used to separate multiple directories
% on a path string for the current platform (basically, a semicolon for Windows,
% and a regular colon  for all other OSes).  The syntax is simple:
% 
%   x = pathsep
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = pathsep
if (strcmp(computer,'PCWIN'))
  x = ';';
else
  x = ':';
end
