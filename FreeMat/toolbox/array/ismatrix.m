% ISMATRIX Test For a 2D Matrix
% 
% Usage
% 
% This function tests to see if the argument is a matrix.  The 
% syntax for ismatrix is
% 
%    x = ismatrix(y)
% 
% and it returns a logical 1 if the argument is size N x M or
% M x N and a logical 0 otherwise.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = ismatrix(x)
  y = (ndims(x)==2);
  
  
