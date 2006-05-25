function s = deblank(S1)
%DEBLANK Remove trailing blanks.
%   R = DEBLANK(S) removes any leading and trailing whitespace characters from string S.  

%   M version contributor: M.W. Vogel 01-03-06

s = strtrim(S1);
