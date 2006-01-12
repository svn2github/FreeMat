%!
%@Module ISREAL Test For Real Array
%@@Section ARRAY
%@@Usage
%The syntax for @|isreal| is 
%@[
%   x = isreal(y)
%@]
%and it returns a logical 1 if the argument is a real type
%(integer, float, or double), and a logical 0 otherwise.
%!
function x = isreal(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','float','double','string'},class(y)));
