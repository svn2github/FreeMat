% PINV Moore-Penrose Pseudoinverse
% 
% Usage
% 
% Calculates the Moore-Penrose pseudoinverse of a matrix.
% The general syntax for its use is
% 
%    y = pinv(A,tol)
% 
% or for a default specification of the tolerance tol,
% 
%    y = pinv(A)
% 
% For any m x n matrix A, the Moore-Penrose pseudoinverse
% is the unique n x m matrix B that satisfies the following
% four conditions
%   -  A B A = A
% 
%   -  B A B = B
% 
%   -  (A B)' = A B
% 
%   -  (B A)' = B A
% 
% Also, it is true that B y is the minimum norm, least squares
% solution to A x = y.  The Moore-Penrose pseudoinverse is computed
% from the singular value decomposition of A, with singular values
% smaller than tol being treated as zeros.  If tol is not specified
% then it is chosen as
% 
%   tol = max(size(A)) * norm(A) * teps(A).
% 
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = pinv(A,tol)
if (isempty(A))
  y = A;
  return;
end
[u,s,v] = svd(A,0);
if (~isset('tol'))
  tol = max(size(A))*s(1,1)*teps(A);
end
sd = diag(s);
sd(sd > tol) = 1./(sd(sd > tol));
s = diag(sd);
y = v*s*u';
