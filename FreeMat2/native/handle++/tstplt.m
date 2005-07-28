path('/localhome/basu/FreeMat2/MFiles');
a = plot2d;
p.x = linspace(0,1);
p.y = cos(2*pi*p.x);
p.color = 'r';
p.symbol = '';
p.line = '-';
p.symbolLength = 3;
p = dataset2d(p);
setholdflag(a,1);
for j = 1:10
  p.y = cos(2*pi*p.x+j*2*pi/100);
if(j==5)
  p.symbol = 'x';
end
  addplot(a,p);
end
p.x = [0,0.25,inf,0.5,0.75];
p.y = [0,0.5,inf,0,0.5];
addplot(a,p);
setholdflag(a,0);
setgrid(a,1);
cfigure('ondraw',a);

