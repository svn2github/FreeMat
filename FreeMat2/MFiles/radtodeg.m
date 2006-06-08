%!
%@Module RADTODEG Radians To Degrees Conversion Function
%@@Section MATHFUNCTIONS
%@@Usage
%Converts the argument array from radians to degrees.  The general
%syntax for its use is
%@[
%   y = radtodeg(x)
%@]
%Note that the output type will be the same as the input type, and that
%complex arguments are allowed.  The output is not wrapped to @|[0,360)|.
%@@Examples
%Some known conversion factors
%@<
%radtodeg(1) % one radian is about 57 degrees
%radtodeg(pi/4) % should be 45 degrees
%radtodeg(2*pi) % Note that this is 360 not 0 degrees
%@>
%!
function y = radtodeg(x)
  if (isa(y,'float') || isa(y,'complex'))
    y = y*float(180/pi);
  else
    y = y*180/pi;
  end
  
  

