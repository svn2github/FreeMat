function tjit22
A = zeros(3000);
for i=1:3000
  for j=1:3000
    A(j,i) = i-j;
  end
end


