%!
%@Module SIND Sine Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the sine of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|cos|). The syntax for its use is
%@[
%   y = sind(x)
%@]
%@@Examples
%The sine of 45 degrees should be @|sqrt(2)/2|
%@<
%sind(45)
%@>
%and the sine of @|30| degrees should be 0.5:
%@<
%sind(30)
%@>
%@@Tests
%@$"y=sind(45)","0.70710678118655","close"
%@$"y=sind(30)","0.5","close"
%!
function y = sind(x)
  if (nargin == 0 || ~isnumeric(x))
    error('sind expects a numeric input');
  end
  y = sin(deg2rad(x));
