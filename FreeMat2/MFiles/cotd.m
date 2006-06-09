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
%!
function y = cotd(x)
  if (nargin == 0 || ~isnumeric(x))
    error('cotd expects a numeric input');
  end
  y = cot(deg2rad(x));
