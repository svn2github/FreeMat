%!
%@Module ISSTR Test For Character Array (string)
%@@Section INSPECTION
%@@Usage
%The syntax for @|isstr| is 
%@[
%   x = isstr(y)
%@]
%and it returns a logical 1 if the argument is a string
%and a logical 0 otherwise.
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = isstr(y)
  x = isa(y,'string');

