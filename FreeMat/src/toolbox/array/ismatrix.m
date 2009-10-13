%!
%@Module ISMATRIX Test For a 2D Matrix
%@@Section INSPECTION
%@@Usage
%This function tests to see if the argument is a matrix.  The 
%syntax for @|ismatrix| is
%@[
%   x = ismatrix(y)
%@]
%and it returns a logical 1 if the argument is size @|N x M| or
%@|M x N| and a logical 0 otherwise.
%@@Tests
%@$exact#y1=ismatrix(rand(3,4))
%@$exact#y1=ismatrix(rand(4,2))
%@$exact#y1=ismatrix(rand(4,4,2))
%@$exact#y1=ismatrix(rand(3,3,3))
%@$exact#y1=ismatrix(rand(1,1,3))
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = ismatrix(x)
  y = (ndims(x)==2);
  
  
