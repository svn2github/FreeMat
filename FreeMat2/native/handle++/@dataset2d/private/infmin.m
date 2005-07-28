function t = infmin(s)
  t = min(s(find(~isinf(s) & ~isnan(s))));
