
% Copyright 2004-2005 Brian Yanoff
function B = squeeze(A)
  sz = size(A);
  sz(find(sz==1)) = [];
  if length(sz)==1
    sz = [1, sz];
  end
  B = reshape(A,sz);
  

  
