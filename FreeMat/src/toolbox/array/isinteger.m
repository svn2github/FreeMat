%!
%@Module ISINTEGER Test for Integer Array
%@@Section ARRAY
%@@Usage
%The syntax for @|isinteger| is
%@[
%  x = isinteger(y)
%@]
%and it returns a logical 1 if the argument is an integer.
%The decision of whether the argument is an integer or not
%is made based on the class of @|y|, not on its value.
%@@Tests
%@{ test_isinteger1.m
%function test_val = test_isinteger1
%  test_val = ~isinteger(single(3)) && isinteger(uint8(3));
%@}
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = isinteger(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','uint64','int64'},class(y)));
