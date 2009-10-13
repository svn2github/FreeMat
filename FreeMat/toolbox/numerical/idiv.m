% IDIV IDIV Integer Division Operation
% 
% Usage
% 
% Computes the integer division of two arrays.  The syntax for its use is
% 
%    y = idiv(a,b)
% 
% where a and b are arrays or scalars.  The effect of the idiv
% is to compute the integer division of b into a.
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = idiv(a,b)
  y = fix(a./b);
