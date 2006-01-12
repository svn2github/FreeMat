%!
%@Module MOD Modulus Operation
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the modulus of an array.  The syntax for its use is
%@[
%   y = mod(x,n)
%@]
%where @|x| is matrix, and @|n| is the base of the modulus.  The
%effect of the @|mod| operator is to add or subtract multiples of @|n|
%to the vector @|x| so that each element @|x_i| is between @|0| and @|n|
%(strictly).  Note that @|n| does not have to be an integer.  Also,
%@|n| can either be a scalar (same base for all elements of @|x|), or a
%vector (different base for each element of @|x|).
%@@Example
%The following examples show some uses of @|mod|
%arrays.
%@<
%mod(18,12)
%mod(6,5)
%mod(2*pi,pi)
%@>
%Here is an example of using @|mod| to determine if integers are even
% or odd:
%@<
%mod([1,3,5,2],2)
%@>
%Here we use the second form of @|mod|, with each element using a 
%separate base.
%@<
%mod([9 3 2 0],[1 0 2 2])
%@>
%!
% Copyright (c) 2005 Samit Basu
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
function y = mod(x,n)
  m = ceil(-x./n);
  y = x + m.*n;
