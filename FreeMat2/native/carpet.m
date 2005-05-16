function carpet(x,y,z,intr)
global ymax ymin xl xh nstot lacc
global thetax thetaz x0 y0 x1 y1 iflag0 iflag1
p = [x;y;z];
R = [cos(thetaz*pi/180),sin(thetaz*pi/180),0;
     -sin(thetaz*pi/180),cos(thetaz*pi/180),0;
     0,0,1];
p = R*p;
R = [1,0,0;
     0,cos(thetax*pi/180),sin(thetax*pi/180);
     0,-sin(thetax*pi/180),cos(thetax*pi/180);];
p = R*p;
x1 = p(1);
y1 = p(2);
if (intr == 0)
  move(x1,y1);
  iflag0 = 1;
  x0 = x1;
  y0 = y1;
  iflag0 = iflag1;
  return;
end

visible(x1,y1,iflag1);
if (iflag0==1 & iflag1==1)
  draw(x1,y1);
  mask(x0,y0,x1,y1);
else
  drawln(x0,y0,x1,y1,lacc,iflag0,iflag1);
  mask(x0,y0,x1,y1);
end
x0 = x1;
y0 = y1;
iflag0 = iflag1;

function move(x,y)
global lx ly
lx = x;
ly = y;

function draw(x,y)
global lx ly
plot([lx,x],[-ly,-y]);
lx = x;
ly = y;

function order(xx0,yy0,xx1,yy1,&x0,&y0,&x1,&y1)
if (xx0 <= xx1)
  x0 = xx0;
  y0 = yy0;
  x1 = xx1;
  y1 = yy1;
else
  x1 = xx0;
  y1 = yy0;
  x0 = xx1;
  y0 = yy1;
end

function mask(xx0,yy0,xx1,yy1)
global ymax ymin xl xh nstot lacc
x0 = 0; y0 = 0; x1 = 0; y1 = 0;
order(xx0,yy0,xx1,yy1,x0,y0,x1,y1);
ns0 = floor((x0-xl)*nstot/(xh-xl)+1);
ns1 = floor((x1-xl)*nstot/(xh-xl)+1);
if (ns0 < 1) ns0 = 1; end
if (ns1 > nstot) ns1 = nstot; end
if (ns0 ~= ns1)
  for n=ns0:ns1
    y = y0 + (y1-y0)*(n-ns0)/(ns1-ns0);
    if (y > ymax(n)), ymax(n) = y; end;
    if (y < ymin(n)), ymin(n) = y; end;
  end
  return
end
if (y0 > ymax(ns0)) ymax(ns0) = y0; end
if (y0 < ymin(ns0)) ymin(ns0) = y0; end
if (y1 > ymax(ns0)) ymax(ns0) = y1; end
if (y1 < ymin(ns0)) ymin(ns0) = y1; end
return

function visible(x,y,&iflag)
global ymax ymin xl xh nstot lacc
iflag = 0;
ns = floor(nstot*(x-xl)/(xh-xl)+1.0);
if (ns > nstot) ns = nstot; end
if (ns < 1) ns = 1; end
if (y >= ymax(ns)) iflag = 1; end;
if (y <= ymin(ns)) iflag = 1; end;


function drawln(xa,ya,xb,yb,lacc,iflag0,iflag1)
if (iflag0 == 1)
  x0 = xa;
  y0 = ya;
  x1 = xb;
  y1 = yb;
else
  x0 = xb;
  y0 = yb;
  x1 = xa;
  y1 = ya;
end

iflag = 0;
for i=1:lacc
  x = (x0+x1)/2;
  y = (y0+y1)/2;
  visible(x,y,iflag);
  if (iflag == 0)
    x1 = x;
    y1 = y;
  else
    x0 = x;
    y0 = y;
  end
end
if (iflag0 == 1 & iflag1 == 0)
  draw(x0,y0);
else if (iflag0 == 0 & iflag1 == 1)
    move(x0,y0);
    draw(xb,yb);
end
end


