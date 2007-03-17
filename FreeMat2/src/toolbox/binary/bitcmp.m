%!
%@Module BITCMP Bitwise Boolean Complement Operation
%@@Section BINARY
%@@Usage
%Performs a bitwise binary complement operation on the argument and
%returns the result.  The syntax for its use is
%@[
%   y = bitcmp(a)
%@]
%where @|a| is an unsigned integer arrays.  This version of the command
%uses as many bits as required by the type of @|a|.  For example, if 
%@|a| is an @|uint8| type, then the complement is formed using 8 bits.
%The second form of @|bitcmp| allows you to specify the number of bits
%to use, 
%@[
%   y = bitcmp(a,n)
%@]
%in which case the complement is taken with respect to @|n| bits. 
%@@Example
%Generally, the bitwise complement of a number is known as its 
%ones-complement.  Here are some examples.  First we take the binary
%complement using 8 bits.
%@<
%bitcmp(uint8(55))
%@>
%Then the complement using 16 bits
%@<
%bitcmp(uint16(55))
%@>
%Finally, we look for the 4 bit complement
%@<
%bitcmp(3,4)
%@>
%@@Tests
%@$"y=bitcmp(uint8(55))","200","exact"
%@$"y=bitcmp(uint16(55))","65480","exact"
%!
function y = bitcmp(a,n)
    if (nargin == 1)
        if (isa(a,'int8') || (isa(a,'uint8')))
            n = 8;
        elseif (isa(a,'int16') || (isa(a,'uint16')))
            n = 16;
        elseif (isa(a,'int32') || (isa(a,'uint32')))
            n = 32;
        else
            n = 32;
        end
    end
    y = cast(bin2int(~int2bin(a,n)),typeof(a));
  
  
