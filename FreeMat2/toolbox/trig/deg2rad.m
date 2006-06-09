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
%@{"y=deg2rad(45)","pi/4","close"}
%!
% Copyright (c) 2002-2006 Samit Basu

function y = deg2rad(x)
  if (isa(x,'float') || isa(x,'complex'))
    y = x*float(pi/180);
  else
    y = x*pi/180;
  end
  
