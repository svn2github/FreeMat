%!
%@Module COSD Cosine Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the cosine of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|cos|). The syntax for its use is
%@[
%   y = cosd(x)
%@]
%@@Examples
%The cosine of 45 degrees should be @|sqrt(2)/2|
%@<
%cosd(45)
%@>
%and the cosine of @|60| degrees should be 0.5:
%@<
%cosd(60)
%@>
%@@Tests
%@$"y=cosd(45)","0.70710678118655","close"
%@$"y=cosd(60)","0.5","close"
%!
function y = cosd(x)
  if (nargin == 0 || ~isnumeric(x) || ~isreal(x))
    error('cosd expects a numeric input');
  end
  y = cos(deg2rad(x));
