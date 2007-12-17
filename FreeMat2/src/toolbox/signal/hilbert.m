%!
%@Module HILBERT Hilbert Transform
%@@Section TRANSFORMS
%@@Usage
%The @|hilbert| function computes the hilbert transform of the argument
%vector or matrix.  The FreeMat @|hilbert| function is compatible with
%the one from the MATLAB API.  This means that the output of the
%@|hilbert| function is the sum of the original function and an
%imaginary signal containing the Hilbert transform of it.  There are
%two syntaxes for the hilbert function.  The first is
%@[
%  y = hilbert(x)
%@]
%where @|x| is real vector or matrix.  If @|x| is a matrix, then he
%Hilbert transform is computed along the columns of @|x|.
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = hilbert(x,n)
  [t,ns] = shiftdim(x);
  if (nargin == 1)
    n = size(t,1);
  end
  f = fft(t,n,1);
  h = zeros(n,1);
  h(1) = 1;
  if (rem(n,2) == 0)
    h((n/2)+1) = 1;
    h(2:(n/2)) = 2;
  else
    h(2:((n+1)/2)) = 2;
  end
  y = ifft(f.*repmat(h,1,size(f,2)));
  y = shiftdim(y,-ns);

