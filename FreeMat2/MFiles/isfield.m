
function y = isfield(x,field)
  if (isa(x,'struct'))
    y = any(strcmp(fieldnames(x),field));
  else
    y = logical(0);
  end
