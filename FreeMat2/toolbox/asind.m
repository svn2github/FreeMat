%!
%@Module ASIND Inverse Sine Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse sine of the argument, but returns
%the argument in degrees instead of radians (as is the case
%for @|asin|). The syntax for its use is
%@[
%   y = asind(x)
%@]
%@@Examples
%The inverse sine of @|sqrt(2)/2| should be 45 degrees:
%@<
%asind(sqrt(2)/2)
%@>
%and the inverse sine of @|0.5| should be 30 degrees:
%@<
%asind(0.5)
%@>
%!
function y = asind(x)
  if (nargin == 0 || ~isnumeric(x))
    error('asind expects a numeric input');
  end
  y = rad2deg(asin(x));
