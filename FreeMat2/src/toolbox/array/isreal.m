%!
%@Module ISREAL Test For Real Array
%@@Section INSPECTION
%@@Usage
%The syntax for @|isreal| is 
%@[
%   x = isreal(y)
%@]
%and it returns a logical 1 if the argument is a real type
%(integer, float, or double), and a logical 0 otherwise.
%!


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = isreal(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','uint64','int64','float','double','string'},class(y)));
