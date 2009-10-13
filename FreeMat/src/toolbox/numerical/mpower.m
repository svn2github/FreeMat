%!
%@Module MPOWER Matrix Power Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the matrix power operator for two arrays.  It is an
%M-file version of the @|^| operator.  The syntax for its use is
%@[
%   y = mpower(a,b)
%@]
%where @|y=a^b|.  See the @|matrixpower| documentation for more
%details on what this function actually does.
%!
function y = mpower(a,b)
  y = a^b;
  
