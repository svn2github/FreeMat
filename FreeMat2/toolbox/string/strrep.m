%!
%@Module STRREP String Replace Function
%@@Section STRING
%@@Usage
%Replace every occurance of one string with another.  The
%general syntax for its use is
%@[
%  p = strrep(source,find,replace)
%@]
%Every instance of the string @|find| in the string @|source| is
%replaced with the string @|replace|.  Any of @|source|, @|find|
%and @|replace| can be a cell array of strings, in which case
%each entry has the replace operation applied.
%@@Example
%Here are some examples of the use of @|strrep|.  First the case
%where are the arguments are simple strings
%@<
%strrep('Matlab is great','Matlab','FreeMat')
%@>
%And here we have the replace operation for a number of strings:
%@<
%strrep({'time is money';'A stitch in time';'No time for games'},'time','money')
%@>
%@@Tests
%@{"y=strrep('Matlab is great','Matlab','FreeMat')","'FreeMat is great'","exact"}
%@{"y=strrep({'time is money';'A stitch in time';'No time for games'},'time','money')","{'money is money';'A stitch in money';'No money for games'}","exact"}
%!

function y = strrep(source,pattern,replace)
  if (isstr(source) & isstr(pattern) & isstr(replace))
    y = strrep_string(source,pattern,replace);
  else
    y = strrep_cell(cellstr(source),...
	            cellstr(pattern),...
	            cellstr(replace));
  end

function y = strrep_cell(source,pattern,replace)
  if (isscalar(source) & isscalar(pattern))
    dsize = size(replace);
  elseif (isscalar(source) & isscalar(replace))
    dsize = size(pattern);
  elseif (isscalar(pattern) & isscalar(replace))
    dsize = size(source);
  elseif (isscalar(source))
    if (any(size(pattern) ~= size(replace)))
      error('All cell-array arguments must be the same size (or scalars)');
    end
    dsize = size(pattern);
  elseif (isscalar(pattern))
    if (any(size(source) ~= size(replace)))
      error('All cell-array arguments must be the same size (or scalars)');
    end
    dsize = size(source);
  elseif (isscalar(replace))
    if (any(size(source) ~= size(pattern)))
      error('All cell-array arguments must be the same size (or scalars)');
    end
    dsize = size(source);
  else
    if (any(size(source) ~= size(pattern)) | any(size(source) ~= size(replace)))
      error('All cell-array arguments must be the same size (or scalars)');
    end
    dsize = size(source);
  end
  if (isscalar(source))
    source = repmat(source,dsize);
  end
  if (isscalar(pattern))
    pattern = repmat(pattern,dsize);
  end
  if (isscalar(replace))
    replace = repmat(replace,dsize);
  end
  y = cell(dsize);
  for i=1:numel(source)
    y{i} = strrep_string(source{i},pattern{i},replace{i});
  end

