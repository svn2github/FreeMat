%!
%@Module CHAR Convert to character array or string
%@@Section Array
%@@Usage
%The @|char| function can be used to convert an array
%into a string.  It has several forms.  The first form
%is
%@[
%   y = char(x)
%@]
%where @|x| is a numeric array containing character codes.
%FreeMat does not currently support Unicode, so the
%character codes must be in the range of @|[0,255]|.  The
%output is a string of the same size as @|x|.  A second
%form is
%@[
%   y = char(c)
%@]
%where @|c| is a cell array of strings, creates a matrix string
%where each row contains a string from the corresponding cell array.
%The third form is
%@[
%   y = char(s1, s2, s3, ...)
%@]
%where @|si| are a character arrays.  The result is a matrix string
%where each row contains a string from the corresponding argument.
%@@Example
%Here is an example of the first technique being used to generate
%a string containing some ASCII characters
%@<
%char([32:64;65:97])
%@>
%In the next example, we form a character array from a set of
%strings in a cell array.  Note that the character array is padded
%with spaces to make the rows all have the same length.
%@<
%char({'hello','to','the','world'})
%@>
%In the last example, we pass the individual strings as explicit
%arguments to @|char|
%@<
%char('hello','to','the','world')
%@>
%!
function y = char(varargin)
  if ((nargin == 1) && ~iscell(varargin{1}))
    y = string(varargin{1});
  elseif ((nargin == 1))
    y = char_block(varargin{1});
  else
    y = char_block(varargin);
  end
  
function y = char_block(x)
  if (~iscell(x))
    error('char_block requires a cell-array argument');
  end
  allstrings = 1;
  maxlen = 0;
  i = 1;
  while (allstrings && i<=numel(x))
    allstrings = isstr(x{i}) || isempty(x{i});
    maxlen = max(maxlen,numel(x{i}));
    i = i+1;
  end
  if (~allstrings)
    error('expecting all string arguments to char');
  end
  y = [];
  for (i=1:numel(x))
    k = string(x{i}(:)'); p = numel(k);
    y = [y;[k,' '+zeros(1,maxlen-p)]];
  end
  
