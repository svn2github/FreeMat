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
%@$exact#y1=isvector(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = isvector(x)
  s = size(x);
  y = (prod(s) == (s(1)*s(2))) && ((s(1) == 1) || (s(2) == 1));
  
  
