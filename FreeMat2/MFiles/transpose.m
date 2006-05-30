%!
%@Module TRANSPOSE Matrix Transpose 
%@@Section ARRAY
%@@Usage
%Performs a (nonconjugate) transpose of a matrix.  The syntax for
%its use is
%@[
%    y = transpose(x)
%@]
%and is a synonym for @|y = x.'|.
%@@Example
%Here is an example of the transpose of a complex matrix.  Note that
%the entries are not conjugated.
%@<
%A = [1+i,2+i;3-2*i,4+2*i]
%transpose(A)
%@>
%!

function y = transpose(x)
    y = x.';
