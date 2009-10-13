%!
%@Module LOWER Convert strings to lower case
%@@Section String
%@@Usage
%The @|lower| function converts a string to lower case with
%the syntax
%@[
%   y = lower(x)
%@]
%where @|x| is a string, in which case all of the upper case
%characters in @|x| (defined as the range @|'A'-'Z'|) are
%converted to lower case.  Alternately, you can call @|lower|
%with a cell array of strings
%@[
%   y = lower(c)
%@]
%in which case each string in the cell array is converted to lower case.
%@@Example
%A simple example:
%@<
%lower('this Is Strange CAPitalizaTion')
%@>
%and a more complex example with a cell array of strings
%@<
%lower({'This','Is','Strange','CAPitalizaTion'})
%@>
%@@Tests
%@$exact#y1=lower(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = lower(x)
  if (isstr(x))
    y = lower_string(x);
  elseif (iscellstr(x))
    y = cell(size(x));
    for i=1:numel(x)
      y{i} = lower_string(x{i});
    end
  else
    y = x;
  end

function y = lower_string(x)
  y = string(x + ('a'-'A')*(x>='A' && x<='Z'));
