%!
%@Module NNZ Number of Nonzeros
%@@Section INSPECTION
%@@Usage
%Returns the number of nonzero elements in a matrix.
%The general format for its use is
%@[
%   y = nnz(x)
%@]
%This function returns the number of nonzero elements
%in a matrix or array.  This function works for both
%sparse and non-sparse arrays.  For 
%@@Example
%@<
%a = [1,0,0,5;0,3,2,0]
%nnz(a)
%A = sparse(a)
%nnz(A)
%@>
%@@Tests
%@$exact#y1=nnz(x1)
%!
function y = nnz(x)
   p = find(x);
   y = numel(p);
