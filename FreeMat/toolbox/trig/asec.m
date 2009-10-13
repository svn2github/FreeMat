% ASEC ASEC Inverse Secant Function
% 
% Usage
% 
% Computes the inverse secant of its argument.  The general
% syntax for its use is
% 
%   y = asec(x)
% 
% where x is an n-dimensional array of numerical type.


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = asec(x)
  y = acos(1 ./ x);
  
