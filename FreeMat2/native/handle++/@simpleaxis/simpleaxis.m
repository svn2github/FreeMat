function this = simpleaxis(a)
  if (nargin == 0)
    this.tMin = 0;
    this.tMax = 1;
    this.tBegin = 0;
    this.tEnd = 1;
    this.tStart = 0;
    this.tStop = 1;
    this.tDelt = 0.1;
    this.tCount = 10;
    this.isLogarithmic = 0;
    this.labels = {};
    this.labelLocations = [];
    this.manualMode = 0;
    this.axisLength = 1000;
    this = class(this,'simpleaxis');
  elseif (strcmp(class(a),'simpleaxis'))
    this = a;
  else
    this = class(a,'simpleaxis');
  end
  
    
