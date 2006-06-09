%!
%@Module SECH Hyperbolic Secant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic secant of the argument.
%The syntax for its use is
%@[
%   y = sech(x)
%@]
%@@Function Internals
%The @|sech| function is computed from the formula
%\[
%   \sech(x) = \frac{1}{\cosh(x)}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic secant function
%@<
%x = -2*pi:.01:2*pi;
%plot(x,sech(x1)); grid('on');
%mprint('sechplot');
%@>
%@figure sechplot
%!
function y = sech(x)
  if (nargin == 0 || ~isnumeric(x))
    error('sech expects a numeric input');
  end
  y = 1.0./sinh(x);
  
