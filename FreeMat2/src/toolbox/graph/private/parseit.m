function b = parseit(&t,dictionary)
  n = stcmp(dictionary,t);
  b = dictionary{min(find(n))};
  t(1:length(b)) = [];
