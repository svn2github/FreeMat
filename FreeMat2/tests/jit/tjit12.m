function tjit12
for i=1:250
  res = 0;
  for j=0:200000
    res = res + sin(j);
    res = res + cos(j);
  end
end

