%!
%@Module FLIPUD Reverse the Columns of a Matrix
%@@Section ARRAY
%@@USAGE
%Reverses the rows of a matrix.  The syntax for its use is
%@[
%   y = flipud(x)
%@]
%where @|x| is matrix.  If @|x| is an N-dimensional array then
%the first dimension is reversed.
%@@Example
%The following example shows @|flipud| applied to a 2D matrix.
%@<
%x = int32(rand(4)*10)
%flipud(x)
%@>
%For a 3D array, note how the rows in each slice are flipped.
%@<
%x = int32(rand(4,4,3)*10)
%flipud(x)
%@>
%!
% Copyright (c) 2002, 2003 Samit Basu
function x = flipud(y)
  x = flipdim(y,1);
