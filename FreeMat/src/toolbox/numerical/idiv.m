%!
%@Module IDIV Integer Division Operation
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the integer division of two arrays.  The syntax for its use is
%@[
%   y = idiv(a,b)
%@]
%where @|a| and @|b| are arrays or scalars.  The effect of the @|idiv|
%is to compute the integer division of @|b| into @|a|.
%@@Example
%The following examples show some uses of @|idiv|
%arrays.
%@<
%idiv(27,6)
%idiv(4,-2)
%idiv(15,3)
%@>
%@@Tests
%@$exact#y1=idiv(x1,6)
%@$exact#y1=idiv(x1,-2)
%!
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = idiv(a,b)
  y = fix(a./b);
