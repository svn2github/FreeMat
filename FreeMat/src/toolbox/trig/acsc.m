%!
%@Module ACSC Inverse Cosecant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse cosecant of its argument.  The general
%syntax for its use is
%@[
%  y = acsc(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acosh| function is computed from the formula
%\[
%   \csc^{-1}(x) = \sin^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse cosecant function
%@<
%x1 = -10:.01:-1.01;
%x2 = 1.01:.01:10;
%plot(x1,acsc(x1),x2,acsc(x2)); grid('on');
%mprint('acscplot');
%@>
%@figure acschplot
%@@Tests
%@$near#y1=acsc(x1)
%!


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acsc(x)
  y = asin(1 ./ x);
  
