% ASECD ASECD Inverse Secant Degrees Function
% 
% Usage
% 
% Computes the inverse secant of the argument, but returns
% the argument in degrees instead of radians (as is the case
% for asec. The syntax for its use is
% 
%    y = asecd(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = asecd(x)
  y = rad2deg(asec(x));
  y(x == 0) = complex(0,inf);
