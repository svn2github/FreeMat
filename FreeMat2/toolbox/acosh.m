%!
%@Module ACOSH Inverse Hyperbolic Cosine Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic cosine of its argument.  The general
%syntax for its use is
%@[
%  y = acosh(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acosh| function is computed from the formula
%\[
%   \acosh(x) = \log\left(x + (x^2 - 1)^0.5\right)
%\]
%where the @|log| (and square root) is taken in its most general sense.
%@@Examples
%Here is a simple plot of the inverse hyperbolic cosine function
%@<
%x = linspace(1,pi);
%plot(x,acosh(x)); grid('on');
%mprint('acoshplot');
%@>
%@figure acoshplot
%!

function y = acosh(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acosh expects a single, numeric input');
  end
  y = log(x+(x.*x-1).^(0.5));
  
