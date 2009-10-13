%!
%@Module ASECD Inverse Secant Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse secant of the argument, but returns
%the argument in degrees instead of radians (as is the case
%for @|asec|. The syntax for its use is
%@[
%   y = asecd(x)
%@]
%@@Examples
%The inverse secant of @|2/sqrt(2)| should be 45 degrees:
%@<
%asecd(2/sqrt(2))
%@>
%and the inverse secant of @|2| should be 60 degrees:
%@<
%asecd(2)
%@>
%@@Tests
%@$near#y1=asecd(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = asecd(x)
  y = rad2deg(asec(x));
  y(x == 0) = complex(0,inf);
