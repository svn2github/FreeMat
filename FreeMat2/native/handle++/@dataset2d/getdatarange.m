function [xmin,xmax,ymin,ymax] = getdatarange(this)
  if (all(isinf(this.x)))
    xmin = -1;
    xmax = 1;
  else
    xmin = min(this.x);
    xmax = max(this.x);
  end
  if (all(isinf(this.y)))
    ymin = -1;
    ymax = 1;
  else
    ymin = min(this.y);
    ymax = max(this.y);
  end
  if ((ymax-ymin) < eps)
    ymin = (ymax+ymin)/2-eps/2;
    ymax = (ymax-ymin)/2+eps/2;
  end
  if (length(this.x) == 1)
    xmin = xmin - 1;
    xmax = xmax + 1;
  end
  if ((xmax-xmin) < eps)
    xmin = (xmax+xmin)/2-eps/2;
    xmax = (xmax-xmin)/2+eps/2;
  end
  
    
