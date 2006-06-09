%!
%@Module TANH Hyperbolic Tangent Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic tangent of the argument.
%The syntax for its use is
%@[
%   y = tanh(x)
%@]
%@@Function Internals
%The @|tanh| function is computed from the formula
%\[
%   \tanh(x) = \frac{\sinh(x)}{\cosh(x)}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic tangent function
%@<
%x = linspace(-5,5);
%plot(x,tanh(x)); grid('on');
%mprint('tanhplot');
%@>
%@figure tanhplot
%!
function y = tanh(x)
  if (nargin == 0 || ~isnumeric(x))
    error('tanh expects a numeric input');
  end
  y = sinh(x)./cosh(x);
  
  
