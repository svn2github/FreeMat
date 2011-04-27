% RANK Calculate the Rank of a Matrix
% 
% Usage
% 
% Returns the rank of a matrix.  There are two ways to use
% the rank function is
% 
%    y = rank(A,tol)
% 
% where tol is the tolerance to use when computing the
% rank.  The second form is
% 
%    y = rank(A)
% 
% in which case the tolerance tol is chosen as
% 
%    tol = max(size(A))*max(s)*eps,
% 
% where s is the vector of singular values of A.  The
% rank is computed using the singular value decomposition svd.
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = rank(A,tol)
if (isempty(A))
  y = 0;
  return;
end
s = svd(A);
if (~isset('tol'))
  tol = max(size(A))*s(1)*teps(A);
end
y = sum(s>tol);

