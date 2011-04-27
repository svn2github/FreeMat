% ISSTRUCT Test For Structure Array
% 
% Usage
% 
% The syntax for isstruct is
% 
%    x = isstruct(y)
% 
% and it returns a logical 1 if the argument is a structure
% array, and a logical 0 otherwise.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = isstruct(y)
  x = isa(y,'struct');

      
