function result = match_exact(a,b)
  if (isempty(a) & isempty(b))
    result = 1;
    return;
  end
  if (~iscell(a))
   result = all(a == b);
  else
    if (numel(a) ~= numel(b))
      result = 0;
      return;
    end
    for i = 1:numel(a)
      result = match_exact(a{i},b{i});
      if (~result) return; end;
    end;
  end
