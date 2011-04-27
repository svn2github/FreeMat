% NORM Norm Calculation
% 
% Usage
% 
% Calculates the norm of a matrix.  There are two ways to
% use the norm function.  The general syntax is
% 
%    y = norm(A,p)
% 
% where A is the matrix to analyze, and p is the
% type norm to compute.  The following choices of p
% are supported
%   -  p = 1 returns the 1-norm, or the max column sum of A
% 
%   -  p = 2 returns the 2-norm (largest singular value of A)
% 
%   -  p = inf returns the infinity norm, or the max row sum of A
% 
%   -  p = 'fro' returns the Frobenius-norm (vector Euclidean norm, or RMS value)
% 
% For a vector, the regular norm calculations are performed:
%   -  1 <= p < inf returns sum(abs(A).^p)^(1/p)
% 
%   -  p unspecified returns norm(A,2)
% 
%   -  p = inf returns max(abs(A))
% 
%   -  p = -inf returns min(abs(A))
% 
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = norm(A,p)
if (~isset('p'))
  p = 2;
end
if (isempty(A))
  y = 0;
  return;
end
if (min(size(A)) == 1)
  y = vector_norm(A,p);
else
  y = matrix_norm(A,p);
end

function y = vector_norm(A,p)
if (p<1 && p~=-inf) 
  error('p argument in vector norm must be between 1 and inf, or be -inf');
end
if (p == inf)
  y = max(abs(A));
elseif (p == -inf)
  y = min(abs(A));
elseif ((p == 2) || strcmp(p,'fro'))
  y = xnrm2(A);
else
  y = sum(abs(A).^p)^(1/p);
end

function y = matrix_norm(A,p)
switch p
  case 1
     y = max(sum(abs(A)));
  case 2
     s = svd(A);
     y = s(1);
  case inf
     y = max(sum(abs(A')));
  case 'fro'
     y = xnrm2(A);
  otherwise
     error('unrecognized norm p-argument');
end


