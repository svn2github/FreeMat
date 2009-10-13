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
%@$near#y1=atand(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = atand(x)
  y = rad2deg(atan(x));
