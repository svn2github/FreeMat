%!
%@Module ASECH Inverse Hyperbolic Secant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic secant of its argument.  The general
%syntax for its use is
%@[
%  y = asech(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|asech| function is computed from the formula
%\[
%   \asech(x) = \cosh^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse hyperbolic secant function
%@<
%x1 = -20:.01:-1;
%x2 = 1:.01:20;
%plot(x1,asech(x1),x2,asech(x2)); grid('on');
%mprint('asechplot');
%@>
%@figure asechplot
%!

function y = asech(x)
  if (nargin == 0 || ~isnumeric(x))
    error('asech expects a single, numeric input');
  end
  y = acosh(1.f./x);
  
