%!
%@Module ACOTH Inverse Hyperbolic Cotangent Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic cotangent of its argument.  The general
%syntax for its use is
%@[
%  y = acoth(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acoth| function is computed from the formula
%\[
%   \acoth(x) = \tanh^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse hyperbolic cotangent function
%@<
%x = linspace(1,pi);
%plot(x,acoth(x)); grid('on');
%mprint('acothplot');
%@>
%@figure acothplot
%!

function y = acoth(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acoth expects a single, numeric input');
  end
  y = atanh(1.f./x);
  
