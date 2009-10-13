%!
%@Module ISSQUARE Test For a Square matrix
%@@Section INSPECTION
%@@Usage
%This function tests to see if the argument is a square matrix.  The 
%syntax for @|issquare| is
%@[
%   x = issquare(y)
%@]
%and it returns a logical 1 if the argument is size @|N x N|
%logical 0 otherwise.
%@@Tests
%@$exact#y1=issquare(rand(4,4))
%@$exact#y1=issquare(rand(4,1,1,1))
%@$exact#y1=issquare(rand(4,3))
%@$exact#y1=issquare(rand(3,3,3))
%@$exact#y1=issquare(rand(1,1,3))
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = issquare(x)
  s = size(x);
  y = isempty(x) || (( s(1) == s(2) ) && ismatrix( x ));
  
  
