% ASIND Inverse Sine Degrees Function
% 
% Usage
% 
% Computes the inverse sine of the argument, but returns
% the argument in degrees instead of radians (as is the case
% for asin). The syntax for its use is
% 
%    y = asind(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = asind(x)
  y = rad2deg(asin(x));
