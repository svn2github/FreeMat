%!
%@Module POWER Element-wise Power Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the element-wise power operator for two arrays.  It is an
%M-file version of the @|.^| operator.  The syntax for its use is
%@[
%   y = power(a,b)
%@]
%where @|y=a.^b|.  See the @|dotpower| documentation for more
%details on what this function actually does.
%!
function y = power(a,b)
  y = a.^b;
  
