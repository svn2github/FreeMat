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
%   \acsc(x) = \sin^{-1}\left(\frac{1}{x}\right)
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
%!

function y = acsc(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acsc expects a single, numeric input');
  end
  y = asin(1.f./x);
  
