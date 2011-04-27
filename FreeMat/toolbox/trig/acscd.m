% ACSCD Inverse Cosecant Degrees Function
% 
% Usage
% 
% Computes the inverse cosecant of the argument, but returns
% the argument in degrees instead of radians (as is the case
% for acsc. The syntax for its use is
% 
%    y = acscd(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acscd(x)
  y = rad2deg(acsc(x));
