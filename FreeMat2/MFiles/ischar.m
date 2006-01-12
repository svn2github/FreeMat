%!
%@Module ISCHAR Test For Character Array (string)
%@@Section ARRAY
%@@Usage
%The syntax for @|ischar| is 
%@[
%   x = ischar(y)
%@]
%and it returns a logical 1 if the argument is a string
%and a logical 0 otherwise.
%!
function x = ischar(y)
  x = isa(y,'string');

