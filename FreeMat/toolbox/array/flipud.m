% FLIPUD FLIPUD Reverse the Columns of a Matrix
% 
% Usage
% 
% Reverses the rows of a matrix.  The syntax for its use is
% 
%    y = flipud(x)
% 
% where x is matrix.  If x is an N-dimensional array then
% the first dimension is reversed.
% Copyright (c) 2002, 2003 Samit Basu
% Licensed under the GPL

function x = flipud(y)
  x = flipdim(y,1);
