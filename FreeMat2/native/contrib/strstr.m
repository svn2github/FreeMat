function s = strstr(S1,S2)
%STRSTR Finds a string within another.
%   S = STRREP(S1,S2) finds all occurrences of the string S2 in
%   string S1.  A list of indices is returned.
%
%   Example:
%       s1='This is a good example';
%       strrep(s1,'good') returns 11
%       strrep(s1,'bad')  returns []
%       strrep(s1,'')     returns []
%

%   M version contributor: M.W. Vogel 12-25-05

if ((isa(S1, 'string') ~= 1) | (isa(S2, 'string') ~= 1) | ...
    (numel(S1) == 0) | (numel(S2) == 0))
   disp('strstr() needs two string arguments.')
   return
end

n = length(S2);

s = S1 == S2(1);

for i=2:n
  s = s & circshift(S1 == S2(i),[1, 1-i]);
end

s = find(s);