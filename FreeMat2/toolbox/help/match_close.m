function result = match_close(a,b)
  if (isa(a,'double') || isa(a,'dcomplex'))
    result = max(abs(a-b)./max(1,abs(a))) < 1e-14;
  else
    result = max(abs(a-b)./max(1,abs(a))) < 1e-6;
  end
