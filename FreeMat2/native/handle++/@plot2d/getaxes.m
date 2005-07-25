function [x1,x2,y1,y2] = getaxes(this)
  [x1,x2] = getaxisextents(this.xaxis);
  [y1,y2] = getaxisextents(this.yaxis);
  
