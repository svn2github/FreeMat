function drawme(&this,plt2d)
  % Draw the symbols
  setpencolor(this,0);
  for i=1:length(x)
    if (~isinf(x(i)) && ~isinf(y(i)))
      [xp,yp] = mappoint(plt2d,x(i),y(i));
      plt2d.putsymbol(xp,yp,this.symbol,this.symbolLength);
    end
  end
  % Draw the lines
  setpencolor(this,1)
  pts = [];
  for i=1:length(x)
    if (~isinf(x(i)) && ~isinf(y(i)))
      [xp,yp] = mappoint(plt2d,x(i),y(i));
      pts = [pts,[xp;yp]];
    else
      drawlines(pts);
      pts = [];
    end
  end
  drawlines(pts);
  
