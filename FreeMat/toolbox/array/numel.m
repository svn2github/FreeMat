% NUMEL Number of Elements in an Array
% 
% Usage
% 
% Returns the number of elements in an array x, or in a subindex
% expression.  The syntax for its use is either
% 
%    y = numel(x)
% 
% or 
% 
%    y = numel(x,varargin)
% 
% Generally, numel returns prod(size(x)), the number of total
% elements in x.  However, you can specify a number of indexing
% expressions for varagin such as index1, index2, ..., indexm.
% In that case, the output of numel is 
% prod(size(x(index1,...,indexm))).

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function len = numel(x,varargin)
if (nargin==1)
  len = prod(size(x));
else
  len = 1;
  for k=1:length(varargin)
    len = len * numel(varargin{k});
  end
end
