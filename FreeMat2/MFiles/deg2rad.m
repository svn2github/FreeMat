%!
%@Module DEG2RAD Convert From Degrees To Radians
%@@Section MATHFUNCTIONS
%@@Usage
%Converts the argument from degrees to radians.  The
%syntax for its use is
%@[
%   y = deg2rad(x)
%@]
%where @|x| is a numeric array.  Conversion is done by
%simply multiplying @|x| by @|pi/180|.
%@@Example
%How many radians in a circle:
%@<
%deg2rad(360) - 2*pi
%@>
%!

% Copyright (c) 2002-2006 Samit Basu

function y = deg2rad(x)
  y = x * pi/180;
  
