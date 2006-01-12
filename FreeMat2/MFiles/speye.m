%!
%@MODULE SPEYE Sparse Identity Matrix
%@@Section SPARSE
%@@Usage
%Creates a sparse identity matrix of the given size.  The syntax for
%its use is
%@[
%  y = speye(m,n)
%@]
%which forms an @|m x n| sparse matrix with ones on the main diagonal,
%or
%@[
%  y = speye(n)
%@]
%which forms an @|n x n| sparse matrix with ones on the main diagonal.  The
%matrix type is a @|float| single precision matrix.
%@@Example
%The following creates a 5000 by 5000 identity matrix, which would be
%difficult to do using @|sparse(eye(5000))| because of the large amount
%of intermediate storage required.
%@<
%I = speye(5000)
%full(I(1:10,1:10))
%@>
%!
function a = speye(n,m)
if (nargin == 1)
  m = n;
end
m = int32(m);
n = int32(n);
if ((m <= 0) | (n <= 0))
  error('size arguments to speye function must be positive integers');
end
a = sparse(1:m,1:n,1.0f);
