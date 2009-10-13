% IFFTN IFFTN N-Dimensional Inverse FFT 
% 
% Usage
% 
% Computes the inverse DFT of an N-dimensional numerical array along all
% dimensions.  The general syntax for its use is
% 
%   y = ifftn(x)
% 
% which computes the same-size inverse  FFTs for each dimension of x.
% Alternately, you can specify the size vector
% 
%   y = ifftn(x,dims)
% 
% where dims is a vector of sizes.  The array x is zero padded
% or truncated as necessary in each dimension so that the output
% is of size dims. The ifftn function is implemented by a sequence
% of calls to ifft.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = ifftn(x,dims)
  if (nargin == 1)
    dims = size(x);
  end
  dims = dims(:)';
  if (numel(dims) < ndims(x))
    pnts = size(x);
    dims = [dims,pnts((numel(dims)+1):end)];
  end
  y = x;
  if (~isa(y,'single')) y = double(y); end;
  if (numel(y) == 0) return; end;
  for n=1:numel(dims)
    y = ifft(y,dims(n),n);
  end
