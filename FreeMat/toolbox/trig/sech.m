% SECH SECH Hyperbolic Secant Function
% 
% Usage
% 
% Computes the hyperbolic secant of the argument.
% The syntax for its use is
% 
%    y = sech(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = sech(x)
  y = 1.0 ./cosh(x);
  
