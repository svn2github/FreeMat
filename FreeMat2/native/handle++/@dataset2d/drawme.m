function drawme(&this,plt2d)
  % Draw the symbols
  setpencolor(this,0);
  for i=1:length(this.x)
    if (~isinf(this.x(i)) && ~isinf(this.y(i)))
      [xp,yp] = mappoint(plt2d,this.x(i),this.y(i));
%      putsymbol(plt2d,xp,yp,this.symbol,this.symbolLength);
    end
  end
  % Draw the lines
  setpencolor(this,1);
  pts = [];
  for i=1:length(this.x)
    if (~isinf(this.x(i)) && ~isinf(this.y(i)))
      [xp,yp] = mappoint(plt2d,this.x(i),this.y(i));
      pts = [pts,[xp;yp]];
    else
      drawlines(pts);
      pts = [];
    end
  end
  drawlines(pts);

