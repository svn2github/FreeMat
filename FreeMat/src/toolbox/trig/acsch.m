%!
%@Module ACSCH Inverse Hyperbolic Cosecant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic cosecant of its argument.  The general
%syntax for its use is
%@[
%  y = acsch(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acsch| function is computed from the formula
%\[
%   \mathrm{csch}^{-1}(x) = \sinh^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse hyperbolic cosecant function
%@<
%x1 = -20:.01:-1;
%x2 = 1:.01:20;
%plot(x1,acsch(x1),x2,acsch(x2)); grid('on');
%mprint('acschplot');
%@>
%@figure acschplot
%@@Tests
%@$near#y1=acsch(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acsch(x)
  y = asinh(1 ./ x);
  
