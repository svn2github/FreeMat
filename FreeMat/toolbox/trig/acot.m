% ACOT ACOT Inverse Cotangent Function
% 
% Usage
% 
% Computes the inverse cotangent of its argument.  The general
% syntax for its use is
% 
%   y = acot(x)
% 
% where x is an n-dimensional array of numerical type.

function y = acot(x)
  y = atan(1.0 ./ x);
  
