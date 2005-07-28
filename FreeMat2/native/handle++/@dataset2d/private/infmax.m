function t = infmax(s)
  t = max(s(find(~isinf(s) & ~isnan(s))));
