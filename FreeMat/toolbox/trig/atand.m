% ATAND Inverse Tangent Degrees Function
% 
% Usage
% 
% Computes the inverse tangent of the argument, but returns
% the argument in degrees instead of radians (as is the case
% for atan. The syntax for its use is
% 
%    y = atand(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = atand(x)
  y = rad2deg(atan(x));
