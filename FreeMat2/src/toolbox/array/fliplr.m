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
%@$"y=fliplr([1,2,3])","[3,2,1]","exact"
%@$"y=fliplr([1;2;3])","[1;2;3]","exact"
%@$"y=fliplr([])","[]","exact"
%!
% Copyright (c) 2002, 2003 Samit Basu
function x = fliplr(y)
  x = flipdim(y,2);
