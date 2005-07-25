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
    drawtextstringaligned(this.xlabel,[mean([xc_min,xc_max]),yc_min+space+sze_textheight+space],'center','top');
  end
  if (~isempty(this.ylabel))
    drawtextstringaligned(this.ylabel,[space+sze_textheight,mean([yc_min,yc_max])],'center','top',90);
  end
  
  
