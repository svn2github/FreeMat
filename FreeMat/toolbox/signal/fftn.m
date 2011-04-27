% FFTN N-Dimensional Forward FFT 
% 
% Usage
% 
% Computes the DFT of an N-dimensional numerical array along all
% dimensions.  The general syntax for its use is
% 
%   y = fftn(x)
% 
% which computes the same-size FFTs for each dimension of x.
% Alternately, you can specify the size vector
% 
%   y = fftn(x,dims)
% 
% where dims is a vector of sizes.  The array x is zero padded
% or truncated as necessary in each dimension so that the output
% is of size dims. The fftn function is implemented by a sequence
% of calls to fft.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = fftn(x,dims)
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
    y = fft(y,dims(n),n);
  end
