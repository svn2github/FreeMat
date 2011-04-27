% ACSCH Inverse Hyperbolic Cosecant Function
% 
% Usage
% 
% Computes the inverse hyperbolic cosecant of its argument.  The general
% syntax for its use is
% 
%   y = acsch(x)
% 
% where x is an n-dimensional array of numerical type.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acsch(x)
  y = asinh(1 ./ x);
  
