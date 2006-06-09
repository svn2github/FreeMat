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
%!
function y = atand(x)
  if (nargin == 0 || ~isnumeric(x))
    error('atand expects a numeric input');
  end
  y = rad2deg(atan(x));
