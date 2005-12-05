
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
