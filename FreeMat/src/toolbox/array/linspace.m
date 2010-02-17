%!
%@Module LINSPACE Linearly Spaced Vector
%@@Section ARRAY
%@@Usage
%Generates a row vector with the specified number of elements, with
%entries uniformly spaced between two specified endpoints.  The syntax
%for its use is either
%@[
%   y = linspace(a,b,count)
%@]
%or, for a default @|count = 100|,
%@[
%   y = linspace(a,b);
%@]
%@@Examples
%Here is a simple example of using @|linspace|
%@<
%x = linspace(0,1,5)
%@>
%@@Tests
%@$exact#z=linspace(0,60000,60001);y1=z(end)
%@$exact#y1=linspace(0,1)
%@$exact#y1=linspace(1,91,91)
%!
% Copyright (c) 2002, 2003 Samit Basu
% Licensed under the GPL
function y = linspace(a,b,len)
  if (nargin < 3)
    len = 100;
  end
  y = a + ((0:(len-1))*(b-a)/(len-1));

