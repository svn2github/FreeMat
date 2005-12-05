
function y = strfind(source,pattern)
  patlen = length(pattern);
  if (isa(source,'string'))
    y = strfind_string(source,pattern);
  elseif (isa(source,'cell'))
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
  
