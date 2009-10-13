% ISFLOAT ISFLOAT Test for Floating Point Array
% 
% Usage
% 
% The syntax for isfloat is
% 
%    x = isfloat(y)
% 
% and it returns a logical 1 if the argument is a floating 
% point array (i.e., a single or double), and a logical
% 0 otherwise.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = isfloat(y)
  x = any(strcmp({'single','double'},class(y)));
  
  
