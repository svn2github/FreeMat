%!
%@Module BIN2DEC Convert Binary String to Decimal
%@@Section ARRAY
%@@USAGE
%Converts a binary string to an integer.  The syntax for its
%use is
%@[
%   y = bin2dec(x)
%@]
%where @|x| is a binary string. If @|x| is a matrix, then the resulting 
%@|y| is a column vector.
%@@Example
%Here we convert some numbers to bits
%@<
%bin2dec('101110')
%bin2dec('010')
%@>
%!
function t = bin2dec(x)
  t = bin2int(x == '1');
