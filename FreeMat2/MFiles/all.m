
function y = all(A,dim)
  if (nargin == 0)
    error 'all function requires at least one argument'
  end
  if (nargin == 1)
    y = min(logical(A(:)));
  else
    y = min(logical(A),dim);
  end
