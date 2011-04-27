% CIRCSHIFT Circularly Shift an Array
% 
% Usage
% 
% Applies a circular shift along each dimension of a given array.  The
% syntax for its use is
% 
%    y = circshift(x,shiftvec)
% 
% where x is an n-dimensional array, and shiftvec is a vector of
% integers, each of which specify how much to shift x along the
% corresponding dimension.  

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function x = circshift(y,shiftvec)
  szey = size(y);
  ndim = prod(size(szey));
  shiftvec = shiftvec(:);
  shiftlen = prod(size(shiftvec));
  d = {};
  for k=1:ndim
    if (k<=shiftlen)
      shift = shiftvec(k);
    else
      shift = 0;
    end
    d{k} = mod((1:szey(k)) - 1 - shift,szey(k)) + 1;
  end;
  x = y(d{:});
