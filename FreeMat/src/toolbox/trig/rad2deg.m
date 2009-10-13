%!
%@Module RAD2DEG Radians To Degrees Conversion Function
%@@Section MATHFUNCTIONS
%@@Usage
%Converts the argument array from radians to degrees.  The general
%syntax for its use is
%@[
%   y = rad2deg(x)
%@]
%Note that the output type will be the same as the input type, and that
%complex arguments are allowed.  The output is not wrapped to @|[0,360)|.
%@@Examples
%Some known conversion factors
%@<
%rad2deg(1) % one radian is about 57 degrees
%rad2deg(pi/4) % should be 45 degrees
%rad2deg(2*pi) % Note that this is 360 not 0 degrees
%@>
%@@Tests
%@$near#y1=rad2deg(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = rad2deg(x)
  y = x*180/pi;
