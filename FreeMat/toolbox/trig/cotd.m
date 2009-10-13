% COTD COTD Cotangent Degrees Function
% 
% Usage
% 
% Computes the cotangent of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for cot). The syntax for its use is
% 
%    y = cotd(x)
% 
% COTD COTD Cotangent Degrees Function
% 
% Usage
% 
% Computes the cotangent of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for cot). The syntax for its use is
% 
%    y = cotd(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cotd(x)
  y = cot(deg2rad(x));
