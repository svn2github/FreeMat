%!
%@Module CROSS Cross Product of Two Vectors
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the cross product of two vectors.  The general syntax
%for its use is
%@[
%    c = cross(a,b)
%@]
%where @|a| and @|b| are 3-element vectors.
%@@Tests
%@$exact#y1=cross([1,2,3],[7,5,2])
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function C = cross(A,B)
  C(1) = A(2)*B(3) - A(3)*B(2);
  C(2) = - A(1)*B(3) + A(3)*B(1);
  C(3) = A(1)*B(2) - A(2)*B(1);
