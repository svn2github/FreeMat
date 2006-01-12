%!
%@Module ISSTR Test For Character Array (string)
%@@Section ARRAY
%@@Usage
%The syntax for @|isstr| is 
%@[
%   x = isstr(y)
%@]
%and it returns a logical 1 if the argument is a string
%and a logical 0 otherwise.
%!
function x = isstr(y)
  x = isa(y,'string');

