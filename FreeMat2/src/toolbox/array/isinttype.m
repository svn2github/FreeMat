%!
%@Module ISINTTYPE Test For Integer-type Array
%@@Section ARRAY
%@@Usage
%The syntax for @|isinttype| is 
%@[
%   x = isinttype(y)
%@]
%and it returns a logical 1 if the argument is an integer type
%and a logical 0 otherwise.  Note that this function only tests
%the type of the variable, not the value.  So if, for example,
%@|y| is a @|float| array containing all integer values, it will
%still return a logical 0.
%!
function x = isinttype(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','uint64','int64'},class(y)));
