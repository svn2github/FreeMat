%!
%@Module ACOSD Inverse Cosine Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse cosine of the argument, but returns
%the argument in degrees instead of radians (as is the case
%for @|acos|. The syntax for its use is
%@[
%   y = acosd(x)
%@]
%@@Examples
%The inverse cosine of @|sqrt(2)/2| should be 45 degrees:
%@<
%acosd(sqrt(2)/2)
%@>
%and the inverse cosine of @|0.5| should be 60 degrees:
%@<
%acosd(0.5)
%@>
%@@Tests
%@$"y=acosd(0.342)","70.00122819209012","close"
%@$"y=acosd(0.523f)","58.4662971f","close"
%@$"y=acosd(0.5)","60","close"
%!
function y = acosd(x)
  if (nargin == 0 || ~isnumeric(x) || ~isreal(x))
    error('acosd expects a real numeric input');
  end
  y = rad2deg(acos(x));
