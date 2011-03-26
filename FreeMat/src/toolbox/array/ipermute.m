%!
%@Module IPERMUTE Array Inverse Permutation Function
%@@Section ARRAY
%@@Usage
%The @|ipermute| function rearranges the contents of an array according
%to the inverse of the specified permutation vector.  The syntx for 
%its use is
%@[
%   y = ipermute(x,p)
%@]
%where @|p| is a permutation vector - i.e., a vector containing the 
%integers @|1...ndims(x)| each occuring exactly once.  The resulting
%array @|y| contains the same data as the array @|x|, but ordered
%according to the inverse of the given permutation.  This function and
%the @|permute| function are inverses of each other.
%@@Example
%First we create a large multi-dimensional array, then permute it
% and then inverse permute it, to retrieve the original array:
%@<
%A = randn(13,5,7,2);
%size(A)
%B = permute(A,[3,4,2,1]);
%size(B)
%C = ipermute(B,[3,4,2,1]);
%size(C)
%any(A~=C)
%@>
%@@Tests
%@$exact#y1=ipermute(x1,[2,1])
%@$exact#y1=size(ipermute(permute(x1),[3,4,2,1]),[3,4,2,1])
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = ipermute(x,p)
  iperm = zeros(1,length(x));
  iperm(p) = 1:length(p);
  y = permute(x,iperm);
