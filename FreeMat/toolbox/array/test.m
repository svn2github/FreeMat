% TEST Test Function
% 
% Usage
% 
% Tests for the argument array to be all logical 1s.  It is 
% completely equivalent to the all function applied to
% a vectorized form of the input.  The syntax for the test
% function is
% 
%    y = test(x)
% 
% and the result is equivalent to all(x(:)).
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = test(x)
   y=all(x(:));

