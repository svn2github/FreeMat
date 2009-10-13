%!
%@Module COTD Cotangent Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the cotangent of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|cot|). The syntax for its use is
%@[
%   y = cotd(x)
%@]
%@@Examples
%The cotangent of 45 degrees should be 1.
%@<
%cotd(45)
%@>
%@@Tests
%@$near#y1=cotd(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cotd(x)
  y = cot(deg2rad(x));
