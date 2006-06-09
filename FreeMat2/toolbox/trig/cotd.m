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
%@@Tests
%@{"y=cotd(0.5)","1.145886501293096e+02","close"}
%@{"y=cotd(2.1324)","26.85674495465909","close"}
%@{"y=cotd(-3)","-19.08113668772821","close"}
%@{"y=cotd(2.12f)","27.0139790f","close"}
%@{"y=cotd(45)","1","exact"}
%!
function y = cotd(x)
  if (nargin == 0 || ~isnumeric(x) || ~isreal(x))
    error('cotd expects a real numeric input');
  end
  y = cot(deg2rad(x));
