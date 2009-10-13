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
%   \mathrm{sech}(x) = \frac{1}{\cosh(x)}
%\]
%@@Examples
%Here is a simple plot of the hyperbolic secant function
%@<
%x = -2*pi:.01:2*pi;
%plot(x,sech(x)); grid('on');
%mprint('sechplot');
%@>
%@figure sechplot
%@@Tests
%@$near#y1=sech(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = sech(x)
  y = 1.0 ./cosh(x);
  
