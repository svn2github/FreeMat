% LENGTH LENGTH Length of an Array
% 
% Usage
% 
% Returns the length of an array x.  The syntax for its use
% is 
% 
%    y = length(x)
% 
% and is defined as the maximum length of x along any of its
% dimensions, i.e., max(size(x)).  If you want to determine the
% number of elements in x, use the numel function instead.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function len = length(x)
if (isempty(x))
  len = 0;
else
  len = max(size(x));
end
  
