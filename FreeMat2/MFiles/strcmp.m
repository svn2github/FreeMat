
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
