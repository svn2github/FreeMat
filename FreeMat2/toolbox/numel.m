%!
%@Module NUMEL Number of Elements in an Array
%@@Section Array
%@@Usage
%Returns the number of elements in an array @|x|, or in a subindex
%expression.  The syntax for its use is either
%@[
%   y = numel(x)
%@]
%or 
%@[
%   y = numel(x,varargin)
%@]
%Generally, @|numel| returns @|prod(size(x))|, the number of total
%elements in @|x|.  However, you can specify a number of indexing
%expressions for @|varagin| such as @|index1, index2, ..., indexm|.
%In that case, the output of @|numel| is 
%@|prod(size(x(index1,...,indexm)))|.
%@@Example
%For a @|4 x 4 x 3| matrix, the length is @|4|, not @|48|, as you 
%might expect, but @|numel| is @|48|.
%@<
%x = rand(4,4,3);
%length(x)
%numel(x)
%@>
%Here is an example of using @|numel| with indexing expressions.
%@<
%numel(x,1:3,1:2,2)
%@>
%!

% Copyright (c) 2002-2006 Samit Basu

function len = numel(x,varargin)
if (nargin==1)
  len = prod(size(x));
else
  len = prod(size(x(varargin{:})));
end
