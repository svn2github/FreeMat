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
%!
% Copyright (c) 2002, 2003 Samit Basu
%
% Permission is hereby granted, free of charge, to any person obtaining a 
% copy of this software and associated documentation files (the "Software"), 
% to deal in the Software without restriction, including without limitation 
% the rights to use, copy, modify, merge, publish, distribute, sublicense, 
% and/or sell copies of the Software, and to permit persons to whom the 
% Software is furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included 
% in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
% OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
% THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
% FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
% DEALINGS IN THE SOFTWARE.
function x = fliplr(y)
  x = flipdim(y,2);
