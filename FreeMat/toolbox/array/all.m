% ALL ALL All True Function
% 
% Usage
% 
% Reduces a logical array along a given dimension by testing for all
% logical 1s.  The general 
% syntax for its use is
% 
%   y = all(x,d)
% 
% where x is an n-dimensions array of logical type.
% The output is of logical type.  The argument d is 
% optional, and denotes the dimension along which to operate.
% The output y is the same size as x, except that it is 
% singular along the operated direction.  So, for example,
% if x is a 3 x 3 x 4 array, and we all operation along
% dimension d=2, then the output is of size 3 x 1 x 4.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = all(A,dim)
  if (nargin == 0)
    error 'all function requires at least one argument'
  end
  if (nargin == 1)
    if  ~isempty(A) 
      y = min(logical(A));
    else
      if (ndims(A) < 3)
        y = true;
      else
        v=size(A);
        v(min(find(v > 1))) = 1;
        y=logical(ones(v));
      end
    end
  else
    if ~isempty(A)
      y = min(logical(A),[],dim);
    else
      v=size(A);
      v(dim)=1;
      y=logical(ones(v));
    end
  end
