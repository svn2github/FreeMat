%!
%@Module ACSC Inverse Cosecant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse cosecant of its argument.  The general
%syntax for its use is
%@[
%  y = acsc(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acosh| function is computed from the formula
%\[
%   \csc^{-1}(x) = \sin^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse cosecant function
%@<
%x1 = -10:.01:-1.01;
%x2 = 1.01:.01:10;
%plot(x1,acsc(x1),x2,acsc(x2)); grid('on');
%mprint('acscplot');
%@>
%@figure acschplot
%@@Tests
%@{"y=acsc(0.342)","1.57079632679490-1.73547783917348i","close"}
%@{"y=acsc(0.342+0.532i)","0.49192807690924-1.19972722497695i","close"}
%@{"y=acsc(2.0)","0.52359877559830","close"}
%@{"y=acsc(0.523f)","1.5707964f-1.2646196fi","close"}
%!

function y = acsc(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acsc expects a single, numeric input');
  end
  y = asin(1.f./x);
  
