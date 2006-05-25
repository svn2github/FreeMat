function s = strtrim(S1)
%STRTRIM Remove insignificant whitespace.
%   S = STRTRIM(M) removes insignificant whitespace from string M.
%
%   Whitespace characters are the following: V = char([9 10 11 12 13 32]), which
%   return true from ISSPACE(V). Per definition, insignificant leading
%   whitespace leads the first non-whitespace character, and insignificant
%   trailing whitespace follows the last non-whitespace character in a string.
%
%   B = STRTRIM(S1) removes insignificant whitespace from the char array. 
%
%   EXAMPLES:
%       M = STRTRIM(S1) removes whitespace from the front and rear of S1.

%   M version contributor: M.W. Vogel 01-03-05

% because freemat is relatively sparse with text functions, we need to
% create a few utility functions, like 

if (length(S1) == 0)
 s = '';
 return;
end

V = [9 10 11 12 13 32];
whitespace = sprintf('%c%c%c%c%c%c', V(1),V(2),V(3),V(4),V(5),V(6));
%disp(whitespace)
i = 1;
while (numel(strstr(whitespace, S1(i)) == 1) & (i < length(S1)))
 i = i + 1;
end
s = S1(i:end);

% now remove trailing whitespace 
S2 = strrev(s);
i = 1;
while (numel(strstr(whitespace, S2(i)) == 1) & (i < length(S2)))
 i = i + 1;
end
s = S2(i:end);

% reverse back to original orientation 
s = strrev(s);
