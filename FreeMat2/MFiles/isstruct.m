%!
%@Module ISSTRUCT Test For Structure Array
%@@Section ARRAY
%@@Usage
%The syntax for @|isstruct| is
%@[
%   x = isstruct(y)
%@]
%and it returns a logical 1 if the argument is a structure
%array, and a logical 0 otherwise.
%!
function x = isstruct(y)
  x = isa(y,'struct');

      
