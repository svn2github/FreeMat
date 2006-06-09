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
%@@Tests
%@{"y=acscd(0.342)","90.0-99.43555562312426i","close"}
%@{"y=acscd(2)","30","close"}
%@{"y=acscd(0.523f)","90.0f-72.4573669fi","close"}
%@{"y=acscd(0.5)","90.0-75.45612929021689i","close"}
%!
function y = acscd(x)
  if (nargin == 0 || ~isnumeric(x) || ~isreal(x))
    error('acscd expects a real numeric input');
  end
  y = rad2deg(acsc(x));
