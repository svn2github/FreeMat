%!
%@Module TAND Tangent Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the tangent of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|cos|). The syntax for its use is
%@[
%   y = tand(x)
%@]
%@@Examples
%The tangent of 45 degrees should be @|1|
%@<
%tand(45)
%@>
%!
function y = tand(x)
  if (nargin == 0 || ~isnumeric(x))
    error('tand expects a numeric input');
  end
  y = tan(deg2rad(x));
