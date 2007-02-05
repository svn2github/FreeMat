%!
%@Module ATAND Inverse Tangent Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse tangent of the argument, but returns
%the argument in degrees instead of radians (as is the case
%for @|atan|. The syntax for its use is
%@[
%   y = atand(x)
%@]
%@@Examples
%The inverse tangent of @|1| should be 45 degrees:
%@<
%atand(1)
%@>
%@@Tests
%@$"y=atand(0.342)","18.88068796535143","close"
%@$"y=atand(2)","63.43494882292201","close"
%@$"y=atand(0.523f)","27.6095676","close"
%@$"y=atand(1)","45","close"
%!
function y = atand(x)
  if (nargin == 0 || ~isnumeric(x) || ~isreal(x))
    error('atand expects a real numeric input');
  end
  y = rad2deg(atan(x));
