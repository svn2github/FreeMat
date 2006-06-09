%!
%@Module ISLOGICAL Test for Logical Array
%@@Section ARRAY
%@@Usage
%The syntax for @|islogical| is 
%@[
%   x = islogical(y)
%@]
%and it returns a logical 1 if the argument is a logical array
%and a logical 0 otherwise.
%!

% Copyright (c) 2002-2006 Samit Basu

function x = islogical(y)
  x = isa(y,'logical');
