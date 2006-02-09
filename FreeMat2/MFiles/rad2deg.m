%!
%@Module RAD2DEG Convert From Degrees To Radians
%@@Section MATHFUNCTIONS
%@@Usage
%Converts the argument from radians to degrees.  The
%syntax for its use is
%@[
%   y = rad2deg(x)
%@]
%where @|x| is a numeric array.  Conversion is done by
%simply multiplying @|x| by @|180/pi|.
%@@Example
%How many degrees in a circle:
%@<
%rad2deg(2*pi)
%@>
%!

% Copyright (c) 2002-2006 Samit Basu

function y = rad2deg(x)
  y = x * 180/pi;
  
