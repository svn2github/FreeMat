%  RESHAPE Reshape Array
%  
%  
%  USAGE
%  
%  The reshape routine has the following syntax
%  
%     y = reshape(x,dims)
%  
%  where dims is either a vector containing the dimensions of the
%  output, or is a sequence of scalars.  In particular, you can 
%  use the form
%  
%     y = reshape(x,dim1,dim2,...,dimn)
%  
%  or
%  
%     y = reshape(x,[dim1,dim2,...,dimn])
%  
%  Note that for a reshape to be legal, the output array
%  must have the same number of elements in it as the input array.
%  
function x = reshape(y,varargin)
  x = zeros(varargin{:});
  x(:) = y;
