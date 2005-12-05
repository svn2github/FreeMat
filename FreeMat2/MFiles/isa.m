
%Copyright (c) 2005 Samit Basu
function y = isa(x,type)
  if (~strcomp(typeof(type),'string'))
    error('type argument to isa must be a string');
  end
  y = strcomp(class(x),type);
