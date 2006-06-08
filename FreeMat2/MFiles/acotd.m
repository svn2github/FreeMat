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
%!

function y = acotd(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acot expects a single, numeric input');
  end
  y = radtodeg(acot(x));
  
  
