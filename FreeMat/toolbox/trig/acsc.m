% ACSC Inverse Cosecant Function
% 
% Usage
% 
% Computes the inverse cosecant of its argument.  The general
% syntax for its use is
% 
%   y = acsc(x)
% 
% where x is an n-dimensional array of numerical type.


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acsc(x)
  y = asin(1 ./ x);
  
