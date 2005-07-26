function [xc,yc] = mappoint(this,x,y)
  xn = normalize(this.xaxis,x);
  yn = normalize(this.yaxis,y);
  u = this.viewport(1) + xn*this.viewport(3);
  v = this.viewport(2) + (1-yn)*this.viewport(4);
  u = min(4096,max(-4096,u));
  v = min(4096,max(-4096,v));
  xc = u;
  yc = v;

