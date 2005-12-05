
function y = log10(x)
  if (isa(x,'float') | isa(x,'complex'))
    y = log(x)/log(10.0f);
  else
    y = log(x)/log(10.0);
  end

