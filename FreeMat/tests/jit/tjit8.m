function tjit8
A = zeros(1,100,'int32');
B = 5;
for i=1:100;
  C = B + i;
  A(i) = C;
end

