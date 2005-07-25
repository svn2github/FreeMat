function [xmin,xmax,ymin,ymax] = getdatarange(this)
  if (all(isinf(x)))
    xmin = -1;
    xmax = 1;
  else
    xmin = min(x);
    xmax = max(x);
  end
  if (all(isinf(y)))
    ymin = -1;
    ymax = 1;
  else
    ymin = min(y);
    ymax = max(y);
  end
  if ((ymax-ymin) < eps)
    ymin = (ymax+ymin)/2-eps/2;
    ymax = (ymax-ymin)/2+eps/2;
  end
  if (length(x) == 1)
    xmin = xmin - 1;
    xmax = xmax + 1;
  end
  if ((xmax-xmin) < eps)
    xmin = (xmax+xmin)/2-eps/2;
    xmax = (xmax-xmin)/2+eps/2;
  end
  
    
