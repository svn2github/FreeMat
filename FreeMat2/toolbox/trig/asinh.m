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
%   \sinh^{-1}(x) = \log\left(x + (x^2 + 1)^0.5\right)
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
%@@Tests
%@$"y=asinh(0.342)","0.33566131926355","close"
%@$"y=asinh(0.342+0.532i)","0.38409830421438+0.51787212574567i","close"
%@$"y=asinh(1)","0.88137358701954","close"
%@$"y=asinh(-1)","-0.88137358701954","close"
%@$"y=asinh(0.543f)","0.5193379f","close"
%!

function y = asinh(x)
  if (nargin == 0 || ~isnumeric(x))
    error('asinh expects a single, numeric input');
  end
  if (isinttype(x)) x = double(x); end
  y = log(x+(x.*x+1.0f).^(0.5f));
  
