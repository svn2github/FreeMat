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
%  p = strcmp(cellstr,y)
%@]
%where @|cellstr| is a cell array of a strings.
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
%!

% Copyright (c) 2002-2006 Samit Basu

function y = strcmp(source,pattern)
  patlen = length(pattern);
  if (isa(source,'string'))
    y = strcomp(source,pattern);
  elseif (isa(source,'cell'))
    y = logical(zeros(size(source)));
    for (i=1:numel(source))
      y(i) = strcomp(source{i},pattern);
    end
  else
    error('strcmp expects string arguments or a cell array of strings');
  end
