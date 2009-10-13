% SPY SPY Visualize Sparsity Pattern of a Sparse Matrix
% 
% Usage
% 
% Plots the sparsity pattern of a sparse matrix.  The syntax for its use is
% 
%    spy(x)
% 
% which uses a default color and symbol.  Alternately, you can use
% 
%    spy(x,colspec)
% 
% where colspec is any valid color and symbol spec accepted by plot.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function spy(A,colspec)
if (nargin==1)
  colspec = 'r.';
end
[i,j] = find(A);
plot(i,j,colspec)
xlabel(sprintf('nz = %d',int32(nnz(A))));
set(gca,'ydir','reverse');



