%!
%Module BITOR Bitwise Boolean Or Operation
%@@Section BINARY
%@@Usage
%Performs a bitwise binary or operation on the two arguments and
%returns the result.  The syntax for its use is
%@[
%   y = bitor(a,b)
%@]
%where @|a| and @|b| are unsigned integer arrays.  The @|or| operation
%is performed using 32 bit unsigned intermediates.  Note that if @|a|
%or @|b| is a scalar, then each element of the other array is ored with
%that scalar.  Otherwise the two arrays must match in size.
%@@Example
%Here we OR some arrays together
%@<
%bitor([3 4 2 3 10 12],5)
%@>
%This is a nice trick to look for odd numbers
%@<
%bitor([3 4 2 3 10 12],1)
%@>
%!

function y = bitor(a,b)
  if (isscalar(a))
    a = repmat(a,size(b));
  end
  if (isscalar(b))
    b = repmat(b,size(a));
  end
  if (any(size(a) ~= size(b)))
    error('Argument arrays must be the same size');
  end
  y = bin2int(int2bin(a,32) | int2bin(b,32));
  
  
