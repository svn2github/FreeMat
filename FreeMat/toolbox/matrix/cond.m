% COND Condition Number of a Matrix
% 
% Usage
% 
% Calculates the condition number of a matrix.  To compute the
% 2-norm condition number of a matrix (ratio of largest to smallest
% singular values), use the syntax
% 
%    y = cond(A)
% 
% where A is a matrix.  If you want to compute the condition number
% in a different norm (e.g., the 1-norm), use the second syntax
% 
%    y = cond(A,p)
% 
% where p is the norm to use when computing the condition number.
% The following choices of p are supported
%   -  p = 1 returns the 1-norm, or the max column sum of A
% 
%   -  p = 2 returns the 2-norm (largest singular value of A)
% 
%   -  p = inf returns the infinity norm, or the max row sum of A
% 
%   -  p = 'fro' returns the Frobenius-norm (vector Euclidean norm, or RMS value)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cond(A,p)
    if (nargin < 2)
        p = 2;
    end
    if (nargin < 1)
        error('expect at least 1 input to cond function');
    end
    if (isempty(A)) y = 0; return; end;
    if (p ~= 2)
        y = norm(A,p)*norm(inv(A),p);
    else
        if (isscalar(A) && (A == 0))
          y = inf;
          return;
        end
        s = svd(A);
        y = s(1)/s(end);
    end
    
