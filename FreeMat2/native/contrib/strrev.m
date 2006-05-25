function s = strrev(S1)
%STRREV reverses string.
%   S = STRTRIM(M) returns reversed string M.
   s = S1;
   n = length(S1);
   for i=1:n
     s(i) = S1(1+n-i);
%S1(i)
   end
