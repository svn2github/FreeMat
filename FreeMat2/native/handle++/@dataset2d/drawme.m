function drawme(&this,plt2d)
  % Draw the symbols
  setpencolor(this,1);
  [xp,yp] = mappoint(plt2d,this.x,this.y);
  drawlines([xp;yp]);
  setpencolor(this,0);
  drawsymbols([xp;yp],this.symbol,this.symbolLength)
