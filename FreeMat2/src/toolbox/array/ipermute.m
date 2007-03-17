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
%@$"y=ipermute([1,2;3,4],[2,1])","[1,3;2,4]","exact"
%@$"y=size(ipermute(permute(randn(13,5,7,2),[3,4,2,1]),[3,4,2,1]))","[13,5,7,2]","exact"
%!
function y = ipermute(x,p)
  iperm = zeros(1,ndims(x));
  iperm(p) = 1:ndims(x);
  y = permute(x,iperm);
