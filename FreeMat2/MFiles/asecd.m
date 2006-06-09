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
%asecd(0.5)
%@>
%!
function y = asecd(x)
  if (nargin == 0 || ~isnumeric(x))
    error('asecd expects a numeric input');
  end
  y = rad2deg(asec(x));
