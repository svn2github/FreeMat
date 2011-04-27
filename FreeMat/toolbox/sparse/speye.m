% SPEYE Sparse Identity Matrix
% 
% Usage
% 
% Creates a sparse identity matrix of the given size.  The syntax for
% its use is
% 
%   y = speye(m,n)
% 
% which forms an m x n sparse matrix with ones on the main diagonal,
% or
% 
%   y = speye(n)
% 
% which forms an n x n sparse matrix with ones on the main diagonal.  The
% matrix type is a float single precision matrix.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function a = speye(n,m)
if (nargin == 1)
  m = n;
end
if ((m <= 0) | (n <= 0))
  error('size arguments to speye function must be positive integers');
end
a = sparse(1:m,1:n,1);
