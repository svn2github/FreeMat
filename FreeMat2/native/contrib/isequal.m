function s = isequal(A1,A2)
%ISEQUAL True if arrays are numerically equal.

%   M version contributor: M.W. Vogel 01-03-06

s = 0;

if ( numel(A1) ~= numel(A2) )
   disp('isequal() needs two array arguments of the same size.')
   return
end

n = length(A2);

s = A1(1) == A2(1);

for i=2:n
  s = s & A1(i) == A2(i);
end
