%!
%@Module ISSPACE Test for Space Characters in a String
%@@Section ARRAY
%@@Usage
%The @|isspace| functions returns a logical array that is 1 
%for characters in the argument string that are spaces, and 
%is a logical 0 for characters in the argument that are not
%spaces.  The syntax for its use is
%@[
%   x = isspace(s)
%@]
%where @|s| is a @|string|.  A blank character is considered
%a space, newline, tab, carriage return, formfeed, and vertical
%tab.
%@@Example
%A simple example of @|isspace|:

%!

function x = isspace(s)
  s = int32(string(s));
  x = ((s >=9 ) & (s <= 13)) | (s == 32);
  
