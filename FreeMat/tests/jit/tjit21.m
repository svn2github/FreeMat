function tjit21
A = zeros(3000);
for i=1:3000
  for j=1:3000
    A(i,j) = i-j;
  end
end

