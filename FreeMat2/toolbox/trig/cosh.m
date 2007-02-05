%!
%@Module COSH Hyperbolic Cosine Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic cosine of the argument.
%The syntax for its use is
%@[
%   y = cosh(x)
%@]
%@@Function Internals
%The @|cosh| function is computed from the formula
%\[
%   \cosh(x) = \frac{e^x+e^{-x}}{2}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic cosine function
%@<
%x = linspace(-5,5);
%plot(x,cosh(x)); grid('on');
%mprint('coshplot');
%@>
%@figure coshplot
%@@Tests
%@$"y=cosh(0.5)","1.12762596520638","close"
%@$"y=cosh(2.1324)","4.27681958616421","close"
%@$"y=cosh(3-4i)","-6.58066304055116+7.58155274274654i","close"
%@$"y=cosh(2.12f)","4.2255840f","close"
%!
function y = cosh(x)
  if (nargin == 0 || ~isnumeric(x))
    error('cosh expects a numeric input');
  end
  y = (exp(x)+exp(-x))/2;
  
