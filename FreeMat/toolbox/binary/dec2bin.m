% DEC2BIN Convert Decimal to Binary String
% 
% Usage
% 
% Converts an integer to a binary string.  The syntax for its
% use is
% 
%    y = dec2bin(x,n)
% 
% where x is the positive integer, and n is the number of
% bits to use in the representation.  Alternately, if you leave
% n unspecified, 
% 
%    y = dec2bin(x)
% 
% the minimum number of bits needed to represent x are used.
% If x is a vector, then the resulting y is a character
% matrix.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function t = dec2bin(x,n)
  x = x(:); 
  if (size(x) == [0,0])
      t = string([]);
      return;
  end
  if (~exist('n') && max(x) > 0)
    n = ceil(log2(max(x)+1e-10));
  elseif (~exist('n'))
  t = string(zeros(size(x))+'0');
    return;
  elseif (max(x) == 0)
  t = string(zeros(size(x))+'0');
    return;
  end
  n = max(1,n);
  t = string(int2bin(x,n)+'0');
