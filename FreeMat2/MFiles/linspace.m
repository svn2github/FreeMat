%  LINSPACE Linearly Spaced Vector
%  
%  
%  USAGE
%  
%  Generates a row vector with the specified number of elements, with
%  entries uniformly spaced between two specified endpoints.  The syntax
%  for its use is either
%  
%     y = linspace(a,b,count)
%  
%  or, for a default count = 100,
%  
%     y = linspace(a,b);
%  
%  
% Copyright (c) 2002, 2003 Samit Basu
function y = linspace(a,b,len)
  if (nargin < 3)
    len = 100;
  end
  y = a + (b-a)*(0:(len-1))/(len-1);

