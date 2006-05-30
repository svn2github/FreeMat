%!
%Module BITXOR Bitwise Boolean Exclusive-Or (XOR) Operation
%@@Section BINARY
%@@Usage
%Performs a bitwise binary xor operation on the two arguments and
%returns the result.  The syntax for its use is
%@[
%   y = bitxor(a,b)
%@]
%where @|a| and @|b| are unsigned integer arrays.  The @|xor| operation
%is performed using 32 bit unsigned intermediates.  Note that if @|a|
%or @|b| is a scalar, then each element of the other array is xored with
%that scalar.  Otherwise the two arrays must match in size.
%@@Example
%Here we XOR some arrays together
%@<
%bitxor([3 4 2 3 10 12],5)
%@>
%This is a nice trick to look for odd numbers
%@<
%bitxor([3 4 2 3 10 12],1)
%@>
%!

function y = bitxor(a,b)
  if (isscalar(a))
    a = repmat(a,size(b));
  end
  if (isscalar(b))
    b = repmat(b,size(a));
  end
  if (any(size(a) ~= size(b)))
    error('Argument arrays must be the same size');
  end
  A = int2bin(a,32); B = int2bin(b,32);
  y = bin2int((A & ~B) | (~A & B));
  
  
