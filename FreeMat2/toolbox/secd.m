%!
%@Module SECD Secant Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the secant of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|sec|). The syntax for its use is
%@[
%   y = secd(x)
%@]
%!
function y = secd(x)
  if (nargin == 0 || ~isnumeric(x))
    error('secd expects a numeric input');
  end
  y = sec(deg2rad(x));
