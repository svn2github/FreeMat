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
%@@Tests
%@$near#y1=deg2rad(x1)
%!


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = deg2rad(x)
  y = x*pi/180;
  
