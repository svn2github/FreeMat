%!
%@Module NONZEROS Retrieve Nonzero Matrix Entries
%@@Section ARRAY
%@@USAGE
%Returns a dense column vector containing the nonzero elements
%of the argument matrix.  The syntax for its use is
%@[
%   y = nonzeros(x)
%@]
%where @|x| is the argument array.  The argument matrix may
%be sparse as well as dense.
%@@Example
%Here is an example of using @|nonzeros| on a sparse matrix.
%@<
%a = rand(8); a(a>0.2) = 0;
%A = sparse(a)
%nonzeros(A)
%@>
%@@Tests
%@$exact#y1=nonzeros(x1)
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = nonzeros(y)
[i,j,x] = find(y);
x = cast(reshape(x,[numel(x),1]),class(y));
