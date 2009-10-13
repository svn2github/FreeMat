% ISALPHA ISALPHA Test for Alpha Characters in a String
% 
% Usage
% 
% The isalpha functions returns a logical array that is 1 
% for characters in the argument string that are letters, and 
% is a logical 0 for characters in the argument that are not
% letters.  The syntax for its use is
% 
%    x = isalpha(s)
% 
% where s is a string.  Note that this function is not
% locale sensitive, and returns a logical 1 for letters in the
% classic ASCII sense (a through z, and A through Z).

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = isalpha(s)
  s = int32(string(s));
  x = (((s >= 65 ) & (s <= 90)) | (s >= 97) & (s <= 122));
  
  
