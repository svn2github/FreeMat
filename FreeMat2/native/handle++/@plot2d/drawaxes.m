function drawaxes(this)
  [xmin,xmax] = getaxisextents(this.xaxis);
  [ymin,ymax] = getaxisextents(this.yaxis);
  [xc_min,yc_min] = mappoint(this,xmin,ymin);
  [xc_max,yc_max] = mappoint(this,xmax,ymax);
  setforegroundcolor([0,0,0]);
  setlinestyle('solid');
  drawline([xc_min,yc_min],[xc_max,yc_min]);
  drawline([xc_min,yc_min],[xc_min,yc_max]);
  if (~isempty(this.xlabel))
     drawtextstringaligned(this.xlabel,[mean([xc_min,xc_max]),...
			yc_min+this.space+this.sze_textheight+this.space],...
			'center','top',0);
  end
  if (~isempty(this.ylabel))
     drawtextstringaligned(this.ylabel,[this.space+this.sze_textheight,...
			mean([yc_min,yc_max])],'center','top',90);
  end
  xtics = getticklocations(this.xaxis);
  xlabels = getticklabels(this.xaxis);
  for (i=1:length(xtics))
    xp = xtics(i);
    [xn,yn] = mappoint(this,xp,ymin);
    drawtextstringaligned(xlabels{i},[xn,yn+this.ticlen],'center','top',0);
    [xn2,yn2] = mappoint(this,xp,ymax);
    if (this.gridFlag & (xn ~= xc_min) & (xn ~= xc_max))
       setforegroundcolor([211,211,211]);
       setlinestyle('dotted');
       drawline([xn,yn],[xn,yn2]);
    end
    setforegroundcolor([0,0,0]);
    setlinestyle('solid');
    drawline([xn,yn],[xn,yn-this.ticlen]);
  end 
  ytics = getticklocations(this.yaxis);
  ylabels = getticklabels(this.yaxis);
  for (i=1:length(ytics))
    yp = ytics(i);
    [xn,yn] = mappoint(this,xmin,yp);
    drawtextstringaligned(ylabels{i},[xn-5,yn],'right','center',0);
    [xn2,yn2] = mappoint(this,xmax,yp);
    if (this.gridFlag & (yn ~= yc_min) & (yn ~= yc_max))
       setforegroundcolor([211,211,211]);
       setlinestyle('dotted');
       drawline([xn,yn],[xn2,yn]);
    end
    setforegroundcolor([0,0,0]);
    setlinestyle('solid');
    drawline([xn,yn],[xn+this.ticlen,yn]);
  end
 
