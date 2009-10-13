%!
%@Module FLIPLR Reverse the Columns of a Matrix
%@@Section ARRAY
%@@USAGE
%Reverses the columns of a matrix.  The syntax for its use is
%@[
%   y = fliplr(x)
%@]
%where @|x| is matrix.  If @|x| is an N-dimensional array then
%the second dimension is reversed.
%@@Example
%The following example shows @|fliplr| applied to a 2D matrix.
%@<
%x = int32(rand(4)*10)
%fliplr(x)
%@>
%For a 3D array, note how the columns in each slice are flipped.
%@<
%x = int32(rand(4,4,3)*10)
%fliplr(x)
%@>
%@@Tests
%@$exact#y1=fliplr(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function x = fliplr(y)
  x = flipdim(y,2);
