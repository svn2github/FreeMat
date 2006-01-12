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
%  p = strncmp(cellstr,y,n)
%@]
%where @|cellstr| is a cell array of a strings.
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
%!
function y = strncmp(source,pattern,n)
  patlen = length(pattern);
  if (isa(source,'string'))
    y = strncmp_string(source,pattern,n);
  elseif (isa(source,'cell'))
    y = logical(zeros(size(source)));
    for (i=1:numel(source))
      y(i) = strncmp_string(source{i},pattern,n);
    end
  else
    error('strncmp expects string arguments or a cell array of strings');
  end

function z = strncmp_string(x,y,n)
  if ((length(x) < n) | (length(y) < n))
    z = logical(0);
  else
    z = strcomp(x(1:n),y(1:n));
  end

