% MOD Modulus Operation
% 
% Usage
% 
% Computes the modulus of an array.  The syntax for its use is
% 
%    y = mod(x,n)
% 
% where x is matrix, and n is the base of the modulus.  The
% effect of the mod operator is to add or subtract multiples of n
% to the vector x so that each element x_i is between 0 and n
% (strictly).  Note that n does not have to be an integer.  Also,
% n can either be a scalar (same base for all elements of x), or a
% vector (different base for each element of x).
% 
% Note that the following are defined behaviors:
% \begin{enumerate}
% \item mod(x,0) = x@
% \item mod(x,x) = 0@
% \item mod(x,n)@ has the same sign as n for all other cases.
% \end{enumerate}
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function y = mod(x,n)
  if (isscalar(x))
    x = repmat(x,size(n));
  end
  if (isscalar(n))
    n = repmat(n,size(x));
  end
  m = floor(x./n);
  y = x - m.*n;
  y(n==0) = x(n==0);
