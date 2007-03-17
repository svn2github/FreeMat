%!
%@Module ATANH Inverse Hyperbolic Tangent Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic tangent of its argument.  The general
%syntax for its use is
%@[
%  y = atanh(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|atanh| function is computed from the formula
%\[
%   \tanh^{-1}(x) = \frac{1}{2}\log\left(\frac{1+x}{1-x}\right)
%\]
%where the @|log| (and square root) is taken in its most general sense.
%@@Examples
%Here is a simple plot of the inverse hyperbolic tangent function
%@<
%x = -0.99:.01:0.99;
%plot(x,atanh(x)); grid('on');
%mprint('atanhplot');
%@>
%@figure atanhplot
%@@Tests
%@$"y=atanh(0.342)","0.35635569310316","close"
%@$"y=atanh(0.342+0.532i)","0.26709380941391+0.52866401400154i","close"
%@$"y=atanh(inf)","1.5707963267949i","close"
%@$"y=atanh(0.523f)","0.5804604f","close"
%!

function y = atanh(x)
  if (nargin == 0 || ~isnumeric(x))
    error('atanh expects a single, numeric input');
  end
  if (isinf(x))
    y = pi/2*i;
  else
    y = 0.5*log((1+x)./(1-x));
  end

  
