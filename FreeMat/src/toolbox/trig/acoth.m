%!
%@Module ACOTH Inverse Hyperbolic Cotangent Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic cotangent of its argument.  The general
%syntax for its use is
%@[
%  y = acoth(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acoth| function is computed from the formula
%\[
%   \coth^{-1}(x) = \tanh^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse hyperbolic cotangent function
%@<
%x = linspace(1,pi);
%plot(x,acoth(x)); grid('on');
%mprint('acothplot');
%@>
%@figure acothplot
%@@Tests
%@$near#y1=acoth(x1)
%!


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acoth(x)
  y = atanh(1 ./ x);
  
