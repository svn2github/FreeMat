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
%@@Tests
%@$"y=tand(0.5)","0.00872686779076","close"
%@$"y=tand(2.1324)","0.03723459420299","close"
%@$"y=tand(-3)","-0.05240777928304","close"
%@$"y=tand(2.12f)","0.0370179f","close"
%@$"y=tand(45)","1","exact"
%!
function y = tand(x)
  if (nargin == 0 || ~isnumeric(x))
    error('tand expects a numeric input');
  end
  y = tan(deg2rad(x));
