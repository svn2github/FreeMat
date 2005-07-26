path('/localhome/basu/FreeMat2/MFiles');
a = plot2d;
p.x = linspace(0,1);
p.y = cos(2*pi*p.x);
p.color = 'r';
p.symbol = '';
p.line = '-';
p.symbolLength = 3;
p = dataset2d(p);
addplot(a,p);
cfigure('ondraw',a);

