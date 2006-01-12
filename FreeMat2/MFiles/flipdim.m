%!
%@Module FLIPDIM Reverse a Matrix Along a Given Dimension
%@@Section ARRAY
%@@USAGE
%Reverses an array along the given dimension.  The syntax for its use is
%@[
%   y = flipdim(x,n)
%@]
%where @|x| is matrix, and @|n| is the dimension to reverse.
%@@Example
%The following examples show some uses of @|flipdim| on N-dimensional
%arrays.
%@<
%x = int32(rand(4,5,3)*10)
%flipdim(x,1)
%flipdim(x,2)
%flipdim(x,3)
%@>
%!
% Copyright (c) 2005 Samit Basu
function x = flipdim(y,n)
  szey = size(y);
  ndim = prod(size(szey));
  n = int32(n);
  if (n<1 | n>ndim), x = y; return; end
  d = {};
  for k=1:ndim
    if (k==n)
      d{k} = szey(k):-1:1;
    else
      d{k} = 1:szey(k);
    end
  end
  x = y(d{:});
