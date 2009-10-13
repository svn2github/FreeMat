% NNZ NNZ Number of Nonzeros
% 
% Usage
% 
% Returns the number of nonzero elements in a matrix.
% The general format for its use is
% 
%    y = nnz(x)
% 
% This function returns the number of nonzero elements
% in a matrix or array.  This function works for both
% sparse and non-sparse arrays.  For 
function y = nnz(x)
   p = find(x);
   y = numel(p);
