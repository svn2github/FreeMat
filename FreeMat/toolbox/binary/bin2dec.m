% BIN2DEC BIN2DEC Convert Binary String to Decimal
% 
% Usage
% 
% Converts a binary string to an integer.  The syntax for its
% use is
% 
%    y = bin2dec(x)
% 
% where x is a binary string. If x is a matrix, then the resulting 
% y is a column vector.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function t = bin2dec(x)
  t = bin2int(x == '1');
