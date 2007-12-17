%!
%@Module ISVECTOR Test For a Vector
%@@Section INSPECTION
%@@Usage
%This function tests to see if the argument is a vector.  The 
%syntax for @|isvector| is
%@[
%   x = isvector(y)
%@]
%and it returns a logical 1 if the argument is size @|N x 1| or
%@|1 x N| and a logical 0 otherwise.
%@@Tests
%@$"y=isvector(rand(1,4))","true","exact"
%@$"y=isvector(rand(4,1))","true","exact"
%@$"y=isvector(rand(4,1,1,1))","true","exact"
%@$"y=isvector(rand(4,4))","false","exact"
%@$"y=isvector(rand(3,3,3))","false","exact"
%@$"y=isvector(rand(1,1,3))","false","exact"
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = isvector(x)
  s = size(x);
  y = isempty(x) || (prod(s) == (s(1)*s(2))) && ((s(1) == 1) || (s(2) == 1));
  
  
