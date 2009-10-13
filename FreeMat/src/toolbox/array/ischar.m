%!
%@Module ISCHAR Test For Character Array (string)
%@@Section INSPECTION
%@@Usage
%The syntax for @|ischar| is 
%@[
%   x = ischar(y)
%@]
%and it returns a logical 1 if the argument is a string
%and a logical 0 otherwise.
%@@Tests
%@$exact#y1=ischar(x1)
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = ischar(y)
  x = isa(y,'char');

