%  LENGTH Length of and Array
%  
%  
%  USAGE
%  
%  Returns the length of an array x.  The syntax for its use
%  is 
%  
%     y = length(x)
%  
%  and is defined as the maximum length of x along any of its
%  dimensions, i.e., max(size(x)).  If you want to determine the
%  number of elements in x, use the numel function instead.
%  
function len = length(x)
len = max(size(x));
