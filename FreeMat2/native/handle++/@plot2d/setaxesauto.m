  if (empty(this.data)) return; end
  [xmin,xmax,ymin,ymax] = getdatarange(data{1});
  for i=2:length(data)
    [txmin,txmax,tymin,tymax] = getdatarange(data{i});
    xmin = min(xmin,txmin);
    xmax = max(xmax,txmax);
    ymin = min(ymin,tymin);
    ymax = max(ymax,tymax);
  end
  setdatarange(this.xaxis,xmin,xmax);
  setdatarange(this.yaxis,ymin,ymax);
