%!
%@MODULE SPY Visualize Sparsity Pattern of a Sparse Matrix
%@@Section SPARSE
%@@Usage
%Plots the sparsity pattern of a sparse matrix.  The syntax for its use is
%@[
%   spy(x)
%@]
%which uses a default color and symbol.  Alternately, you can use
%@[
%   spy(x,colspec)
%@]
%where @|colspec| is any valid color and symbol spec accepted by @|plot|.
%@@Example
%First, an example of a random sparse matrix.
%@<
%y = sprand(1000,1000,.001)
%spy(y,'ro')
%mprint spy1
%@>
%which is shown here
%@figure spy1
%Here is a sparse matrix with a little more structure.  First we build a 
%sparse matrix with block diagonal structure, and then use @|spy| to 
%visualize the structure.
%@<
%A = sparse(1000,1000);
%for i=1:25; A((1:40) + 40*(i-1),(1:40) + 40*(i-1)) = 1; end;
%spy(A,'gx')
%mprint spy2
%@>
%with the result shown here
%@figure spy2
%!
function spy(A,colspec)
if (nargin==1)
  colspec = 'r.';
end
[i,j] = find(A);
plot(i,j,colspec)
xlabel(sprintf('nz = %d',int32(nnz(A))));
set(gca,'ydir','reverse');



