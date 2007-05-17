%!
%@Module ISINTEGER Test for Integer Array
%@@Section ARRAY
%@@Usage
%The syntax for @|isnumeric| is
%@[
%  x = isnumeric(y)
%@]
%and it returns a logical 1 if the argument is an integer.
%The decision of whether the argument is an integer or not
%is made based on the class of @|y|, not on its value.
%!
function x = isinteger(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','uint64','int64'},class(y)));
