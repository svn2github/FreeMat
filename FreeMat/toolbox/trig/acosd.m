% ACOSD ACOSD Inverse Cosine Degrees Function
% 
% Usage
% 
% Computes the inverse cosine of the argument, but returns
% the argument in degrees instead of radians (as is the case
% for acos. The syntax for its use is
% 
%    y = acosd(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acosd(x)
  y = rad2deg(acos(x));
  y(x == inf) = complex(0,inf);
  y(x == -inf) = complex(180,-inf);
