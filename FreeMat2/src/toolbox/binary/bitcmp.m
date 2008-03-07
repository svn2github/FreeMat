% BITCMP BITCMP Bitwise Boolean Complement Operation
% 
% Usage
% 
% Performs a bitwise binary complement operation on the argument and
% returns the result.  The syntax for its use is
% 
%    y = bitcmp(a)
% 
% where a is an unsigned integer arrays.  This version of the command
% uses as many bits as required by the type of a.  For example, if 
% a is an uint8 type, then the complement is formed using 8 bits.
% The second form of bitcmp allows you to specify the number of bits
% to use, 
% 
%    y = bitcmp(a,n)
% 
% in which case the complement is taken with respect to n bits, where n must be 
% less than the length of the integer type.
%
function Out = bitcmp(In, N)
Out = bitcmp_cpp(In);
if(nargin > 1)
  Type = typeof(In);
  Mask = cast((2^N-1), Type);
  switch (Type)
  case 'uint8'
    Length = 8;
  case 'uint16'
    Length = 16;
  case 'uint32'
    Length = 32;
  case 'uint64'
    Length = 64;
  otherwise
    error('Only unsigned integer types are allowed');
  end
  if( N > Length)
    error('Number of converted bits exceeds the bit length of the input type');
  elseif( N < Length)
    Out = bitand(Out, Mask);
    High = bitand(In, bitcmp(Mask));
    Out = bitor(Out, High);
  end
end
