%!
%@Module EYE Identity Matrix
%@@Section ARRAY
%@@USAGE
%Creates an identity matrix of the specified size.  The syntax for
%its use is
%@[
%   y = eye(n)
%@]
%where @|n| is the size of the identity matrix.  The type of the 
%output matrix is @|float|.
%@@Example
%The following example demonstrates the identity matrix.
%@<
%eye(3)
%@>
%@@Tests
%@$exact#y1=eye(3)
%@$exact#y1=eye(300)
%!
% Copyright (c) 2002-2005 Samit Basu
% Licensed under the GPL
function I = eye(n)
I = diag(ones(1,n));
