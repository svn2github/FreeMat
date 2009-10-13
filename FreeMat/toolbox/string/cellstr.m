% CELLSTR CELLSTR Convert character array to cell array of strings
% 
% Usage
% 
% The cellstr converts a character array matrix into a 
% a cell array of individual strings.  Each string in
% the matrix is placed in a different cell, and extra spaces
% are removed.  The syntax for the command is
% 
%    y = cellstr(x)
% 
% where x is an N x M array of characters as a string.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function A = cellstr(x)
  if (iscell(x))
    A = x;
    return;
  end
  if (~isstr(x) | (ndims(x) ~= 2))
    error('Argument to cellstr must be a string matrix');
  end
  n = size(x,1);
  if (n == 1)
      A = {deblank(x)};
  else
      A = cell(n,1);
      for i=1:n
          A{i} = deblank(x(i,:));
      end
  end
  
