%!
%@Module ACOTD Inverse Cotangent Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse cotangent of its argument in degrees.  The general
%syntax for its use is
%@[
%  y = acotd(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Tests
%@{"y=acotd(0.342)","71.11931203464857","close"}
%@{"y=acotd(2)","26.56505117707799","close"}
%@{"y=acotd(0.523f)","62.3904343f","close"}
%@{"y=acotd(1)","45","close"}
%!

function y = acotd(x)
  if (nargin == 0 || ~isnumeric(x) || ~isreal(x))
    error('acot expects a single, real numeric input');
  end
  y = rad2deg(acot(x));
  
  
