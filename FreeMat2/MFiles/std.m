%  STD Standard Deviation Function
%  
%  
%  USAGE
%  
%  Computes the standard deviation of an array along a given dimension.  
%  The general syntax for its use is
%  
%    y = std(x,d)
%  
%  where x is an n-dimensions array of numerical type.
%  The output is of the same numerical type as the input.  The argument
%  d is optional, and denotes the dimension along which to take
%  the variance.  The output y is the same size as x, except
%  that it is singular along the mean direction.  So, for example,
%  if x is a 3 x 3 x 4 array, and we compute the mean along
%  dimension d=2, then the output is of size 3 x 1 x 4.
%  

% Copyright (c) 2002-2005 Samit Basu
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
function y = std(varargin)
  y = sqrt(var(varargin{:}));
