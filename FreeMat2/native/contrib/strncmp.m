function s = strncmp(S1,S2,N)
%STRNCMP Compare first N characters of strings.
%   STRNCMP(S1,S2,N) returns 1 if the first N characters of
%   the strings S1 and S2 are the same and 0 otherwise.
%
%   Example:
%       s1='This is a good example';
%       strncmp(s1,'good',4) returns 0
%       strncmp(s1,'This',4) returns 1
%

%   M version contributor: M.W. Vogel 01-03-05

% because freemat is relatively sparse with text functions, we need to
% create a few utility functions, like 

len = length(S1); 
if (len < N) 
  disp('Error, arguments shorter than expected');
  return;
end

%s = 0;
%val = strstr(S1(1:N), S2(1:N));
%if (numel(val) > 0)
%   s = 1;
%end

s = strcmp(S1(1:N), S2(1:N));
