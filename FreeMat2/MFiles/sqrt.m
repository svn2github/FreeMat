%  SQRT Square Root of an Array
%  
%  
%  USAGE
%  
%  Computes the square root of the argument matrix.  The general
%  syntax for its use is
%  
%     y = sqrt(x)
%  
%  where x is an N-dimensional numerical array.
%  
function y = sqrt(x)
  y = x.^(0.5f);
