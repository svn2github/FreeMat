% REM REM Remainder After Division
% 
% Usage
% 
% Computes the remainder after division of an array.  The syntax for its use is
% 
%    y = rem(x,n)
% 
% where x is matrix, and n is the base of the modulus.  The
% effect of the rem operator is to add or subtract multiples of n
% to the vector x so that each element x_i is between 0 and n
% (strictly).  Note that n does not have to be an integer.  Also,
% n can either be a scalar (same base for all elements of x), or a
% vector (different base for each element of x).
% 
% Note that the following are defined behaviors:
%   1.  rem(x,0) = nan@
% 
%   2.  rem(x,x) = 0@ for nonzero x
% 
%   3.  rem(x,n)@ has the same sign as x for all other cases.
% 
% Note that rem and mod return the same value if x and n
% are of the same sign.  But differ by n if x and y have 
% different signs.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = rem(x,n)
  if (isscalar(x))
    x = repmat(x,size(n));
  end
  if (isscalar(n))
    n = repmat(n,size(x));
  end
  m = fix(x./n);
  y = x - m.*n;
