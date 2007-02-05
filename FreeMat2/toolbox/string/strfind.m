%!
%@Module STRFIND Find Substring in a String
%@@Section STRING
%@@Usage
%Searches through a string for a pattern, and returns the starting
%positions of the pattern in an array.  There are two forms for 
%the @|strfind| function.  The first is for single strings
%@[
%   ndx = strfind(string, pattern)
%@]
%the resulting array @|ndx| contains the starting indices in @|string|
%for the pattern @|pattern|.  The second form takes a cell array of 
%strings
%@[
%   ndx = strfind(cells, pattern)
%@]
%and applies the search operation to each string in the cell array.
%@@Example
%Here we apply @|strfind| to a simple string
%@<
%a = 'how now brown cow?'
%b = strfind(a,'ow')
%@>
%Here we search over multiple strings contained in a cell array.
%@<
%a = {'how now brown cow','quick brown fox','coffee anyone?'}
%b = strfind(a,'ow')
%@>
%@@Tests
%@$"a = 'how now brown cow?';y=strfind(a,'ow')","[2,6,11,16]","exact"
%@$"a = {'how now brown cow','quick brown fox','coffee anyone?'};y=strfind(a,'ow')","{[2,6,11,16],9,[]}","exact"
%!

function y = strfind(source,pattern)
  patlen = length(pattern);
  if (isa(source,'string'))
    y = strfind_string(source,pattern);
  elseif (iscellstr(source))
    y = cell(size(source));
    for (i=1:numel(source))
      y{i} = strfind_string(source{i},pattern);
    end
  else
    error('strfind expects string arguments or a cell array of strings');
  end

function y = strfind_string(source,pattern)
  if (~isa(source,'string') | ~isa(pattern,'string'))
    error('strfind expects string arguments or a cell array of strings');
  end
  patlen = length(pattern);
  y = [];
  p = strstr(source,pattern);
  while ((p > 0) && ~isempty(source))      
    if (~isempty(y))
      y = [y,p+y(end)+patlen-1];
    else
      y = [p];
    end
    source = source((p+patlen):end);
    p = strstr(source,pattern);
  end
  
