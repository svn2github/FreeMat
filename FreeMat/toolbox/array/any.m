% ANY ANY Any True Function
% 
% Usage
% 
% Reduces a logical array along a given dimension by testing for any
% logical 1s.  The general 
% syntax for its use is
% 
%   y = any(x,d)
% 
% where x is an n-dimensions array of logical type.
% The output is of logical type.  The argument d is 
% optional, and denotes the dimension along which to operate.
% The output y is the same size as x, except that it is 
% singular along the operated direction.  So, for example,
% if x is a 3 x 3 x 4 array, and we any operation along
% dimension d=2, then the output is of size 3 x 1 x 4.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = any(A,dim)
  if (nargin == 0)
    error 'any function requires at least one argument'
  end
  A(isnan(A))=0;
  if (nargin == 1)
    if (isempty(A))
      if (ndims(A) < 3)
        y = false;
      else
        v = size(A);
        v(min(find(v > 1))) = 1;
        y = logical(ones(v));
      end
    else
      y = max(logical(A));
    end
 else
    if ~isempty( A )
       y = max(logical(A),[],dim);
    else
       v=size(A);
       v(dim)=1;
       y=zeros(v,'logical');
    end
  end
