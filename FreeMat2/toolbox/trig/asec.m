%!
%@Module ASEC Inverse Secant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse secant of its argument.  The general
%syntax for its use is
%@[
%  y = asec(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acosh| function is computed from the formula
%\[
%   \sec^{-1}(x) = \cos^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse secant function
%@<
%x1 = -5:.01:-1;
%x2 = 1:.01:5;
%plot(x1,asec(x1),x2,asec(x2)); grid('on');
%mprint('asecplot');
%@>
%@figure asecplot
%@@Tests
%@$"y=asec(2.342)","1.12963987411802","close"
%@$"y=asec(0.342+0.532i)","1.07886824988565+1.19972722497695i","close"
%@$"y=asec(inf)","pi/2","close"
%@$"y=asec(2.523f)","1.1632546","close"
%!

function y = asec(x)
  if (nargin == 0 || ~isnumeric(x))
    error('asec expects a single, numeric input');
  end
  y = acos(1.f./x);
  
