% CONV Convolution Function
% 
% Usage
% 
% The conv function performs a one-dimensional convolution of two
% vector arguments.  The syntax for its use is
% 
%      z = conv(x,y)
% 
% where x and y are vectors.  The output is of length nx + ny -1.
% The conv function calls conv2 to do the calculation.  See its
% help for more details.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function z = conv(x,y)
% is x a column vector
% col * scalar = col
% col * row = row
% ndim * row = row
% col * row = col
xiscol = size(x,1) > size(x,2);
yiscol = size(y,1) > size(y,2);
if (xiscol | yiscol)
  z = conv2(x(:),y(:));
else
  z = conv2(x(:).',y(:).');
end
if (numel(x) > numel(y))
  maxdims = maxdim(x);
else
  maxdims = maxdim(y);
end
if (maxdims > 1)
  z = reshape(z,[ones(1,maxdims-1),numel(z)]);
end

