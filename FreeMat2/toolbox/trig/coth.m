%!
%@Module COTH Hyperbolic Cotangent Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic cotangent of the argument.
%The syntax for its use is
%@[
%   y = coth(x)
%@]
%@@Function Internals
%The @|coth| function is computed from the formula
%\[
%   \coth(x) = \frac{1}{\tanh(x)}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic cotangent function
%@<
%x1 = -pi+.01:.01:-.01;
%x2 = .01:.01:pi-.01;
%plot(x1,coth(x1),x2,coth(x2)); grid('on');
%mprint('cothplot');
%@>
%@figure cothplot
%@@Tests
%@{"y=coth(0.5)","2.16395341373865","close"}
%@{"y=coth(2.1324)","1.02851005521481","close"}
%@{"y=coth(3-4i)","0.99926692780590+0.00490118239430i","close"}
%@{"y=coth(2.12f)","1.0292364f","close"}
%!
function y = coth(x)
  if (nargin == 0 || ~isnumeric(x))
    error('coth expects a numeric input');
  end
  y = 1.0f./tanh(x);
  
