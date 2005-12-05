
% Copyright (c) 2005 Samit Basu
function x = circshift(y,shiftvec)
  szey = size(y);
  ndim = prod(size(szey));
  shiftvec = int32(shiftvec);
  shiftvec = shiftvec(:);
  shiftlen = prod(size(shiftvec));
  d = {};
  for k=1:ndim
    if (k<=shiftlen)
      shift = shiftvec(k);
    else
      shift = 0;
    end
    d{k} = int32(mod((1:szey(k)) - 1 - shift,szey(k)) + 1);
  end;
  x = y(d{:});
