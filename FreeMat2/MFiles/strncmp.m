
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

