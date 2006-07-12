%!
%@Module STRCMP String Compare Function
%@@Section STRING
%@@USAGE
%Compares two strings for equality.  The general
%syntax for its use is
%@[
%  p = strcmp(x,y)
%@]
%where @|x| and @|y| are two strings.  Returns @|true| if @|x|
%and @|y| are the same size, and are equal (as strings).  Otherwise,
%it returns @|false|.
%In the second form, @|strcmp| can be applied to a cell array of
%strings.  The syntax for this form is
%@[
%  p = strcmp(cellstra,cellstrb)
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
%strcmp(x1,x2)
%strcmp(x1,x3)
%@>
%Here we use a cell array strings
%@<
%x = {'astring','bstring',43,'astring'}
%p = strcmp(x,'astring')
%@>
%Here we compare two cell arrays of strings
%@<
%strcmp({'this','is','a','pickle'},{'what','is','to','pickle'})
%@>
%Finally, the case where one of the arguments is a matrix
%string
%@<
%strcmp({'this','is','a','pickle'},['peter ';'piper ';'hated ';'pickle'])
%@>
%@@Tests
%@{"y=strcmp('astring','astring')","1","exact"}
%@{"y=strcmp('astring','bstring')","0","exact"}
%@{"x={'astring','bstring',43,'astring'};y=strcmp(x,'astring')","[1,0,0,1]","exact"}
%@{"y=strcmp({'this','is','a','pickle'},{'what','is','to','pickle'})","[0,1,0,1]","exact"}
%@{"y=strcmp({'this','is','a','pickle'},['peter ';'piper ';'hated ';'pickle'])","[0,0,0,1]","exact"}
%!

% Copyright (c) 2002-2006 Samit Basu

function y = strcmp(source,pattern)
  if (isstr(source) & isstr(pattern))
    y = strcmp_string_string(source,pattern);
  else
    y = strcmp_cell_cell(cellstr(source),cellstr(pattern));
  end


function y = strcmp_string_string(source,pattern)
  y = strcomp(source,pattern);

function y = strcmp_cell_cell(source,pattern)
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
    y(i) = strcomp(source{i},pattern{i});
  end
