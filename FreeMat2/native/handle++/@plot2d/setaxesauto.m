function setaxesauto(&this);
  if (isempty(this.data)) return; end
  [xmin,xmax,ymin,ymax] = getdatarange(this.data{1});
  for i=2:length(this.data)
    [txmin,txmax,tymin,tymax] = getdatarange(this.data{i});
    xmin = min(xmin,txmin);
    xmax = max(xmax,txmax);
    ymin = min(ymin,tymin);
    ymax = max(ymax,tymax);
  end
  setdatarange(this.xaxis,xmin,xmax);
  setdatarange(this.yaxis,ymin,ymax);
