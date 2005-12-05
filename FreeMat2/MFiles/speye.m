
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
