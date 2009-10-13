% ACOTH ACOTH Inverse Hyperbolic Cotangent Function
% 
% Usage
% 
% Computes the inverse hyperbolic cotangent of its argument.  The general
% syntax for its use is
% 
%   y = acoth(x)
% 
% where x is an n-dimensional array of numerical type.


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acoth(x)
  y = atanh(1 ./ x);
  
