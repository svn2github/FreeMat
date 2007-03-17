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
%   \coth^{-1}(x) = \tanh^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse hyperbolic cotangent function
%@<
%x = linspace(1,pi);
%plot(x,acoth(x)); grid('on');
%mprint('acothplot');
%@>
%@figure acothplot
%@@Tests
%@$"y=acoth(0.342)","0.35635569310316+1.57079632679490i","close"
%@$"y=acoth(0.342+0.532i)","0.26709380941391-1.04213231279336i","close"
%@$"y=acoth(inf)","0","exact"
%@$"y=acoth(0.523f)","0.5804604f+1.5707964fi","close"
%!

function y = acoth(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acoth expects a single, numeric input');
  end
  y = atanh(1.f./x);
  
