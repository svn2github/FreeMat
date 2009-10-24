% EXPM EXPM Matrix Exponential
% 
% Usage
% 
% Calculates e^A for a square, full rank matrix A.  The
% syntax for its use is
% 
%    y = expm(A)
% 
% Internally, expm is mapped to a simple e^A expression (which
% in turn uses the eigenvalue expansion of A to compute the
% exponential).

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = expm(A)
    y = e^A;
    
