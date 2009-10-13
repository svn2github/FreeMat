% ACOTD ACOTD Inverse Cotangent Degrees Function
% 
% Usage
% 
% Computes the inverse cotangent of its argument in degrees.  The general
% syntax for its use is
% 
%   y = acotd(x)
% 
% where x is an n-dimensional array of numerical type.

function y = acotd(x)
  y = rad2deg(acot(x));
  
  
