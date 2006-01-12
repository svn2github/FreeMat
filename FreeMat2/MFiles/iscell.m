%!
%@Module ISCELL Test For Cell Array
%@@Section ARRAY
%@@Usage
%The syntax for @|iscell| is 
%@[
%   x = iscell(y)
%@]
%and it returns a logical 1 if the argument is a cell array
%and a logical 0 otherwise.
%!
function x = iscell(y)
  x = isa(y,'cell');

