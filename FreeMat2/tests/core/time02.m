B = [1,2;3,4];
A = [3,4;3,2];
C = 2;
D = 9;
E = 32;
%k = 1:10000;
%for i = k;
i = 1;
while (i<10000)
  A = A + B + i;
  C = C - 1;
  D = A + C;
  E = A + C + D;
  i = i + 1;
end
