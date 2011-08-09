% TRANSPOSE Matrix Transpose Operator
% 
% Usage
% 
% Performs a transpose of the argument (a 2D matrix).  The syntax for its use is
% 
%   y = a.';
% 
% where a is a M x N numerical matrix.  The output y is a numerical matrix
% of the same type of size N x M.  This operator is the non-conjugating transpose,
% which is different from the Hermitian operator ' (which conjugates complex values).


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = transpose(x)
    y = x.';
