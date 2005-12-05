
% Copyright (c) 2005 Samit Basu
function y = teps(x)
switch(typeof(x))
  case {'float','complex'}
    y = feps;
  case {'double','dcomplex','int8','uint8','int16','uint16','int32','uint32'}
    y = eps;
  otherwise
    error('teps only applies to numerical arrays');
end
