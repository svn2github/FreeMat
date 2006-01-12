%!
%@Module ISNUMERIC Test for Numeric Array
%@@Section ARRAY
%@@Usage
%The syntax for @|isnumeric| is
%@[
%  x = isnumeric(y)
%@]
%and it returns a logical 1 if the argument is a numeric
%(i.e., not a structure array, cell array, string or user
%defined class), and a logical 0 otherwise.
%!
function x = isnumeric(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','float','double','complex','dcomplex'},class(y)));
