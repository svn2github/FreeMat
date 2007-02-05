%!
%@Module CSCH Hyperbolic Cosecant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic cosecant of the argument.
%The syntax for its use is
%@[
%   y = csch(x)
%@]
%@@Function Internals
%The @|csch| function is computed from the formula
%\[
%   \mathrm{csch}(x) = \frac{1}{\sinh(x)}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic cosecant function
%@<
%x1 = -pi+.01:.01:-.01;
%x2 = .01:.01:pi-.01;
%plot(x1,csch(x1),x2,csch(x2)); grid('on');
%mprint('cschplot');
%@>
%@figure cschplot
%@@Tests
%@$"y=csch(3)","0.09982156966882","close"
%@$"y=csch(-3.5)","-0.06044989000916","close"
%@$"y=csch(1+3*i)","-0.83042813377958-0.15543010515010i","close"
%@$"y=csch(5.234f)","0.0106646f","close"
%!
function y = csch(x)
  if (nargin == 0 || ~isnumeric(x))
    error('csch expects a numeric input');
  end
  y = 1.0f./sinh(x);
  
