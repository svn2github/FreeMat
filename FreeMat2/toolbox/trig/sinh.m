%!
%@Module SINH Hyperbolic Sine Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the hyperbolic sine of the argument.
%The syntax for its use is
%@[
%   y = sinh(x)
%@]
%@@Function Internals
%The @|sinh| function is computed from the formula
%\[
%   \sinh(x) = \frac{e^x+e^{-x}}{2}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic sine function
%@<
%x = linspace(-5,5);
%plot(x,sinh(x)); grid('on');
%mprint('sinhplot');
%@>
%@figure sinhplot
%@@Tests
%@{"y=sinh(0.5)","0.52109530549375","close"}
%@{"y=sinh(2.1324)","4.15826715983928","close"}
%@{"y=sinh(3-4i)","-6.54812004091100+7.61923172032141i","close"}
%@{"y=sinh(2.12f)","4.1055527","close"}
%!
function y = sinh(x)
  if (nargin == 0 || ~isnumeric(x))
    error('sinh expects a numeric input');
  end
  y = (exp(x)-exp(-x))/2;
  
