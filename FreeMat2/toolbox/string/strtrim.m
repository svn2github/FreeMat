%!
%@Module STRTRIM Trim Spaces from a String
%@@Section STRING
%@@Usage
%Removes the white-spaces at the beginning and end of a string (or a 
%cell array of strings). See @|isspace| for a definition of a white-space.
%There are two forms for the @|strtrim| function.  The first is for
%single strings
%@[
%   y = strtrim(strng)
%@]
%where @|strng| is a string.  The second form operates on a cell array
%of strings
%@[
%   y = strtrim(cellstr)
%@]
%and trims each string in the cell array.
%@@Example
%Here we apply @|strtrim| to a simple string
%@<
%strtrim('  lot of blank spaces    ');
%@>
%and here we apply it to a cell array
%@<
%strtrim({'  space','enough ',' for ',''})
%@>
%@@Tests
%@{"y=strtrim('  lot of blank spaces    ')","'lot of blank spaces'","exact"}
%@{"y=strtrim({'  space','enough ',' for ',''})","{'space','enough','for',''}","exact"}
%!

function y = strtrim(x)
  if (isa(x,'string'))
    y = strtrim_string(x);
  elseif (iscellstr(x))
    y = cell(size(x));
    for (i=1:numel(x))
      y{i} = strtrim_string(x{i});
    end
  else
    error('strtrim expects string arguments or a cell array of strings');
  end

function y = strtrim_string(x)
  if (~isa(x,'string'))
    error('strtrim expects string arguments or a cell array of strings');
  end
  ndx = find(~isspace(x(:)));
  p = numel(x);
  todel = [1:(min(ndx)-1),(max(ndx)+1):p];
  y = x;
  y(todel) = [];

