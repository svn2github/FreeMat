%!
%@Module CSCD Cosecant Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the cosecant of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|csc|). The syntax for its use is
%@[
%   y = cscd(x)
%@]
%@@Tests
%@$"y=cscd(45)","1.41421356237309","close"
%@$"y=cscd(60)","1.15470053837925","close"
%!
function y = cscd(x)
  if (nargin == 0 || ~isnumeric(x))
    error('cscd expects a numeric input');
  end
  y = csc(deg2rad(x));
