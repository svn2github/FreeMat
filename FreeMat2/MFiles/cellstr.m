%!
%@Module CELLSTR Convert character array to cell array of strings
%@@Section String
%@@Usage
%The @|cellstr| converts a character array matrix into a 
%a cell array of individual strings.  Each string in
%the matrix is placed in a different cell, and extra spaces
%are removed.  The syntax for the command is
%@[
%   y = cellstr(x)
%@]
%where @|x| is an @|N x M| array of characters as a string.
%@@Example
%Here is an example of how to use @|cellstr|
%@<
%a = ['quick';'brown';'fox  ';'is   ']
%cellstr(a)
%@>
%!
function A = cellstr(x)
  if (iscell(x))
    A = x;
    return;
  end
  if (~isstr(x) | (ndims(x) ~= 2))
    error('Argument to cellstr must be a string matrix');
  end
  n = size(x,1);
  m = size(x,2);
  A = cell(n,1);
  for i=1:n
    ndx = max(find(~isspace(x(i,:))));
    A{i} = x(i,1:ndx);
  end
