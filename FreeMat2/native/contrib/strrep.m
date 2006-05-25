function s = strrep(S1,S2,S3)
%STRREP Replace string with another.
%   S = STRREP(S1,S2,S3) replaces all occurrences of the string S2 in
%   string S1 with the string S3.  The new string is returned.
%
%   Example:
%       s1='This is a good example';
%       strrep(s1,'good','great') returns 'This is a great example'
%       strrep(s1,'bad','great')  returns 'This is a good example'
%       strrep(s1,'','great')     returns 'This is a good example'
%

%   M version contributor: M.W. Vogel 12-25-05

% because freemat is relatively sparse with text functions, we need to
% create a few utility functions, like 

tokens = strstr(S1,S2)-1;
endtok = tokens + length(S2) + 1;
tokens = [tokens; length(S1)];

s_new = S1(1:tokens(1));
for i=1:numel(endtok)
  % build the new string
  s_new = [s_new S3 S1(endtok(i):tokens(i+1))];
end

s = s_new;
