function surface
global ymax ymin xl xh nstot lacc
global thetax thetaz x0 y0 x1 y1 iflag0 iflag1

thetax = -70;
thetaz = 40;
ymax = ones(131,1)*(-inf);
ymin = ones(131,1)*(inf);
nstot = 130;
iflag = 1;
n = 21;
lacc = 8;
xl = -10;
xh = 10;
yl = -8;
yh = 8;
x = ((1:n) - 11)/2;
y = ((1:n) - 11)/2;
for i = 1:n
  for j = 1:n
    r = sqrt(x(i)^2+y(j)^2);
    z(i,j) = sin(r);
  end
end

for i=2:n
  carpet(x(1),y(i),z(1,i),0);
  i1 = i-1;
  for j=1:i1
    k=i-j;
    for m=1:2
      carpet(x(j+m-1),y(k),z(j+m-1,k),1)
    end
  end
end

n1 = n-1;
for i=1:n1
  carpet(x(i),y(n),z(i,n),0);
  k = n;
  i1 = i + 1;
  for j=i1:n
    for m=1:2
      carpet(x(j),y(k),z(j,k),1)
      k = k - 1;
    end
    k = k + 1;
  end
end

	 
