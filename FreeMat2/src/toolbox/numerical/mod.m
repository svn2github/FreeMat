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
%
%Note that the following are defined behaviors:
%\begin{enumerate}
%\item @|mod(x,0) = x|@
%\item @|mod(x,x) = 0|@
%\item @|mod(x,n)|@ has the same sign as @|n| for all other cases.
%\end{enumerate}
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
