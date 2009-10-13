%!
%@Module ACOTD Inverse Cotangent Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse cotangent of its argument in degrees.  The general
%syntax for its use is
%@[
%  y = acotd(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Tests
%@$near#y1=acotd(x1)
%!

function y = acotd(x)
  y = rad2deg(acot(x));
  
  
