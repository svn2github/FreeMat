% FLIPLR FLIPLR Reverse the Columns of a Matrix
% 
% Usage
% 
% Reverses the columns of a matrix.  The syntax for its use is
% 
%    y = fliplr(x)
% 
% where x is matrix.  If x is an N-dimensional array then
% the second dimension is reversed.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function x = fliplr(y)
  x = flipdim(y,2);
