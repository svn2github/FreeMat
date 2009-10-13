%!
%@Module ISFLOAT Test for Floating Point Array
%@@Section ARRAY
%@@Usage
%The syntax for @|isfloat| is
%@[
%   x = isfloat(y)
%@]
%and it returns a logical 1 if the argument is a floating 
%point array (i.e., a @|single| or @|double|), and a logical
%0 otherwise.
%@@Tests
%@{ test_isfloat1.m
%function test_val = test_isfloat1
%  test_val = isfloat(3) && ~isfloat('hello');
%@}
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = isfloat(y)
  x = any(strcmp({'single','double'},class(y)));
  
  
