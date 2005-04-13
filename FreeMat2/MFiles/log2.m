
function y = log2(x)
  if (isa(x,'float') | isa(x,'complex'))
    y = log(x)/log(2.0f);
  else
    y = log(x)/log(2.0);
  end

