% STR2NUM STR2NUM Convert a String to a Number
% 
% Usage
% 
% Converts a string to a number.  The general syntax for its use
% is
% 
%   x = str2num(string)
% 
% Here string is the data string, which contains the data to 
% be converted into a number.  The output is in double precision,
% and must be typecasted to the appropriate type based on what
% you need.  Note that by definition, str2num is entirely 
% equivalent to eval(['[' string ']'],[]) with all of the 
% associated problems where string contains text that causes
% side effects.
function y = str2num(x)
if (~isstr(x)) 
  y = []; 
  return;
end
try
  y = eval(['[',x,']'],'[]');
catch
  y = [];
end
