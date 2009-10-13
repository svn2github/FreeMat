%!
%@Module ASEC Inverse Secant Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse secant of its argument.  The general
%syntax for its use is
%@[
%  y = asec(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acosh| function is computed from the formula
%\[
%   \sec^{-1}(x) = \cos^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse secant function
%@<
%x1 = -5:.01:-1;
%x2 = 1:.01:5;
%plot(x1,asec(x1),x2,asec(x2)); grid('on');
%mprint('asecplot');
%@>
%@figure asecplot
%@@Tests
%@$near#y1=asec(x1)
%!


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = asec(x)
  y = acos(1 ./ x);
  
