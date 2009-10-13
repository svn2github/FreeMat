% RAD2DEG RAD2DEG Radians To Degrees Conversion Function
% 
% Usage
% 
% Converts the argument array from radians to degrees.  The general
% syntax for its use is
% 
%    y = rad2deg(x)
% 
% Note that the output type will be the same as the input type, and that
% complex arguments are allowed.  The output is not wrapped to [0,360).

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = rad2deg(x)
  y = x*180/pi;
