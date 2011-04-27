% FLIPDIM Reverse a Matrix Along a Given Dimension
% 
% Usage
% 
% Reverses an array along the given dimension.  The syntax for its use is
% 
%    y = flipdim(x,n)
% 
% where x is matrix, and n is the dimension to reverse.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = flipdim(y,n)
  if (isempty(y)), x = y; return; end
  szey = size(y);
  ndim = prod(size(szey));
  n = int32(n);
  if (n<1 | n>ndim), x = y; return; end
  d = {};
  for k=1:ndim
    if (k==n)
      d{k} = szey(k):-1:1;
    else
      d{k} = 1:szey(k);
    end
  end
  x = y(d{:});
