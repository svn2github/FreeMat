%!
%@Module ACSCD Inverse Cosecant Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse cosecant of the argument, but returns
%the argument in degrees instead of radians (as is the case
%for @|acsc|. The syntax for its use is
%@[
%   y = acscd(x)
%@]
%@@Examples
%The inverse cosecant of @|2/sqrt(2)| should be 45 degrees:
%@<
%acscd(2/sqrt(2))
%@>
%and the inverse cosecant of @|2| should be 30 degrees:
%@<
%acscd(0.5)
%@>
%@@Tests
%@$near#y1=acscd(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acscd(x)
  y = rad2deg(acsc(x));
