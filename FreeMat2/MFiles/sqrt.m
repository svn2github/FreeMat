%!
%@Module SQRT Square Root of an Array
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the square root of the argument matrix.  The general
%syntax for its use is
%@[
%   y = sqrt(x)
%@]
%where @|x| is an N-dimensional numerical array.
%@@Example
%Here are some examples of using @|sqrt|
%@<
%sqrt(9)
%sqrt(i)
%sqrt(-1)
%x = rand(4)
%sqrt(x)
%@>
%!
function y = sqrt(x)
  y = x.^(0.5f);
