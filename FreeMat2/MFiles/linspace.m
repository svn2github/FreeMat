
% Copyright (c) 2002, 2003 Samit Basu
function y = linspace(a,b,len)
  if (nargin < 3)
    len = 100;
  end
  if (b < a) 
    y = [];
  else
    y = a + (b-a)*(0:(len-1))/(len-1);
  end
