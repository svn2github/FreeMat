
function y = any(A,dim)
  if (nargin == 0)
    error 'any function requires at least one argument'
  end
  if (nargin == 1)
    y = max(logical(A(:)));
  else
    y = max(logical(A),dim);
  end
