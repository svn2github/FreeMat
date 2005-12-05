
function spy(A,colspec)
if (nargin==1)
  colspec = 'r.';
end
[i,j] = find(A);
plot(i,j,colspec)
xlabel(sprintf('nz = %d',int32(nnz(A))));


