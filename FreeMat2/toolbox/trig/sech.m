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
%@@Tests
%@{"y=sech(3)","0.09932792741943","close"}
%@{"y=sech(-3.5)","0.06033974412017","close"}
%@{"y=sech(1+3*i)","-0.64698003387259-0.07023784378464i","close"}
%@{"y=sech(5.234f)","0.0106640f","close"}
%!
function y = sech(x)
  if (nargin == 0 || ~isnumeric(x))
    error('sech expects a numeric input');
  end
  y = 1.0f./sinh(x);
  
