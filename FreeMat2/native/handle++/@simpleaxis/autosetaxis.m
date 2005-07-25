function autosetaxis(&this)
  this.manualMode = 0;
  m = getnominaltickcount(this);
  delt = (this.tMax-this.tMin)/m;
  n = ceil(log10(delt));
  rdelt = delt/10^n;
  p = floor(log2(rdelt));
  this.tDelt = 10^n*2^p;
  this.tStart = floor(this.tMin/this.tDelt)*this.tDelt;
  this.tStop = ceil(this.tMax/this.tDelt)*this.tDelt;
  this.tBegin = this.tStart;
  this.tEnd = this.tStop;
  mprime = ceil((this.tEnd-this.tBegin)/this.tDelt);
  if ((this.tBegin+(mprime-1)*this.tDelt) > this.tMax)
      mprime = mprime - 1;
  end
  this.tCount = mprime+1;
  setticks(this);

  
