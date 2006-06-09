%!
%@Module ACSCH Inverse Hyperbolic Cosecant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic cosecant of its argument.  The general
%syntax for its use is
%@[
%  y = acsch(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acsch| function is computed from the formula
%\[
%   \acsch(x) = \sinh^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse hyperbolic cosecant function
%@<
%x1 = -20:.01:-1;
%x2 = 1:.01:20;
%plot(x1,acsch(x1),x2,acsch(x2)); grid('on');
%mprint('acschplot');
%@>
%@figure acschplot
%@@Tests
%@{"y=acsch(0.342)","1.79412760639059","close"}
%@{"y=acsch(0.342+0.532i)","1.12267740551487-0.89903500983626i","close"}
%@{"y=acsch(inf)","0","exact"}
%@{"y=acsch(0.523f)","1.4035949f","close"}
%!

function y = acsch(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acsch expects a single, numeric input');
  end
  y = asinh(1.f./x);
  
