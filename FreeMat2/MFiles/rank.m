
% Copyright (c) 2005 Samit Basu
function y = rank(A,tol)
s = svd(A);
if (~isset('tol'))
  tol = max(size(A))*s(1)*teps(A);
end
y = sum(s>tol);

