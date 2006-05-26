%!
%@Module STRNCMP String Compare Function To Length N 
%@@Section STRING
%@@USAGE
%Compares two strings for equality, but only looks at the
%first N characters from each string.  The general syntax 
%for its use is
%@[
%  p = strncmp(x,y,n)
%@]
%where @|x| and @|y| are two strings.  Returns @|true| if @|x|
%and @|y| are each at least @|n| characters long, and if the
%first @|n| characters from each string are the same.  Otherwise,
%it returns @|false|.
%In the second form, @|strncmp| can be applied to a cell array of
%strings.  The syntax for this form is
%@[
%  p = strncmp(cellstra,cellstrb,n)
%@]
%where @|cellstra| and @|cellstrb| are cell arrays of a strings
%to compare.  Also, you can also supply a character matrix as
%an argument to @|strcmp|, in which case it will be converted
%via @|cellstr| (so that trailing spaces are removed), before being
%compared.
%@@Example
%The following piece of code compares two strings:
%@<
%x1 = 'astring';
%x2 = 'bstring';
%x3 = 'astring';
%strncmp(x1,x2,4)
%strncmp(x1,x3,4)
%@>
%Here we use a cell array strings
%@<
%x = {'ast','bst',43,'astr'}
%p = strncmp(x,'ast',3)
%@>
%Here we compare two cell arrays of strings
%@<
%strncmp({'this','is','a','pickle'},{'think','is','to','pickle'},3)
%@>
%Finally, the case where one of the arguments is a matrix
%string
%@<
%strcmp({'this','is','a','pickle'},['peter ';'piper ';'hated ';'pickle'],4);
%@>
%!

% Copyright (c) 2002-2006 Samit Basu
function y = strncmp(source,pattern,n)
  if (isstr(source) & isstr(pattern))
    y = strncmp_string_string(source,pattern,n);
  else
    y = strncmp_cell_cell(cellstr(source),cellstr(pattern),n);
  end


function y = strncmp_cell_cell(source,pattern,n)
  if (isscalar(source))
    source = repmat(source,size(pattern));
  end
  if (isscalar(pattern))
    pattern = repmat(pattern,size(source));
  end
  if (numel(source) ~= numel(pattern))
    error('cell array arguments must be the same size')
  end
  y = logical(zeros(size(source)));
  for (i=1:numel(source))
    y(i) = strncmp_string_string(source{i},pattern{i},n);
  end

function z = strncmp_string_string(x,y,n)
  if ((length(x) < n) | (length(y) < n))
    z = logical(0);
  else
    z = strcomp(x(1:n),y(1:n));
  end

