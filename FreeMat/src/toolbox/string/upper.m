%!
%@Module UPPER Convert strings to upper case
%@@Section String
%@@Usage
%The @|upper| function converts a string to upper case with
%the syntax
%@[
%   y = upper(x)
%@]
%where @|x| is a string, in which case all of the lower case
%characters in @|x| (defined as the range @|'a'-'z'|) are
%converted to upper case.  Alternately, you can call @|upper|
%with a cell array of strings
%@[
%   y = upper(c)
%@]
%in which case each string in the cell array is converted to upper case.
%@@Example
%A simple example:
%@<
%upper('this Is Strange CAPitalizaTion')
%@>
%and a more complex example with a cell array of strings
%@<
%upper({'This','Is','Strange','CAPitalizaTion'})
%@>
%@@Tests
%@$exact#y1=upper(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = upper(x)
  if (isstr(x))
    y = upper_string(x);
  elseif (iscell(x))
    y = cell(size(x));
    for i=1:numel(x)
      y{i} = upper_string(x{i});
    end
  else
    y = x;
  end

function y = upper_string(x)
  if (isstr(x))
    y = string(x + ('A'-'a')*(x>='a' && x<='z'));
  else
    y = x;
  end;
