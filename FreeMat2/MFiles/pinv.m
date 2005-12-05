
% Copyright (c) 2005 Samit Basu
function y = pinv(A,tol)
[u,s,v] = svd(A,0);
if (~isset('tol'))
  tol = max(size(A))*s(1,1)*teps(A);
end
sd = diag(s);
sd(sd > tol) = 1.0f/(sd(sd > tol));
s = diag(sd);
y = v*s*u';
