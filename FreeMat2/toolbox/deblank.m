%!
%@Module DEBLANK Remove trailing blanks from a string
%@@Section String
%@@Usage
%The @|deblank| function removes spaces at the end of a string
%when used with the syntax
%@[
%   y = deblank(x)
%@]
%where @|x| is a string, in which case, all of the extra spaces
%in @|x| are stripped from the end of the string.  Alternately,
%you can call @|deblank| with a cell array of strings
%@[
%   y = deblank(c)
%@]
%in which case each string in the cell array is deblanked.
%@@Example
%A simple example
%@<
%deblank('hello   ')
%@>
%and a more complex example with a cell array of strings
%@<
%deblank({'hello  ','there ','  is  ','  sign  '})
%@>
%!
function y = deblank(x)
  if (isstr(x))
    y = deblank_string(x);
  elseif (iscellstr(x))
    y = cell(size(x));
    for i=1:numel(x)
      y{i} = deblank_string(x{i});
    end
  else
    error('deblank expects a string or cell array of strings as an argument');
  end

function y = deblank_string(x)
  ndx = max(find(~isspace(x)));
  y = x(1:ndx);

