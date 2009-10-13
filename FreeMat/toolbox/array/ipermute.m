% IPERMUTE IPERMUTE Array Inverse Permutation Function
% 
% Usage
% 
% The ipermute function rearranges the contents of an array according
% to the inverse of the specified permutation vector.  The syntx for 
% its use is
% 
%    y = ipermute(x,p)
% 
% where p is a permutation vector - i.e., a vector containing the 
% integers 1...ndims(x) each occuring exactly once.  The resulting
% array y contains the same data as the array x, but ordered
% according to the inverse of the given permutation.  This function and
% the permute function are inverses of each other.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = ipermute(x,p)
  iperm = zeros(1,ndims(x));
  iperm(p) = 1:ndims(x);
  y = permute(x,iperm);
