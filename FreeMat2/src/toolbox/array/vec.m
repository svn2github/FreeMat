%!
%@Module VEC Reshape to a Vector
%@@Section ELEMENTARY
%@@Usage
%Reshapes an n-dimensional array into a column vector.  The general
%syntax for its use is
%@[
%   y = vec(x)
%@]
%where @|x| is an n-dimensional array (not necessarily numeric).  This
%function is equivalent to the expression @|y = x(:)|.
%@@Example
%A simple example of the @|vec| operator reshaping a 2D matrix:
%@<
%A = [1,2,4,3;2,3,4,5]
%vec(A)
%@>
%!
function y = vec(x)
  y = x(:);
  
