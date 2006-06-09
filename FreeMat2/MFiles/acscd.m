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
%!
function y = acscd(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acscd expects a numeric input');
  end
  y = rad2deg(acsc(x));
