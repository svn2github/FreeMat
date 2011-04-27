% ISLOGICAL Test for Logical Array
% 
% Usage
% 
% The syntax for islogical is 
% 
%    x = islogical(y)
% 
% and it returns a logical 1 if the argument is a logical array
% and a logical 0 otherwise.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = islogical(y)
  x = isa(y,'logical');
