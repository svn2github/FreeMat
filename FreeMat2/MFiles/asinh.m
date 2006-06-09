%!
%@Module ASINH Inverse Hyperbolic Sine Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic sine of its argument.  The general
%syntax for its use is
%@[
%  y = asinh(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|asinh| function is computed from the formula
%\[
%   \asinh(x) = \log\left(x + (x^2 + 1)^0.5\right)
%\]
%where the @|log| (and square root) is taken in its most general sense.
%@@Examples
%Here is a simple plot of the inverse hyperbolic sine function
%@<
%x = -5:.01:5;
%plot(x,asinh(x)); grid('on');
%mprint('asinhplot');
%@>
%@figure asinhplot
%!

function y = asinh(x)
  if (nargin == 0 || ~isnumeric(x))
    error('asinh expects a single, numeric input');
  end
  y = log(x+(x.*x+1).^(0.5));
  
