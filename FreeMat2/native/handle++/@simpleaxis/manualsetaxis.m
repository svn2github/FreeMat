function manualsetaxis(&this,t1,t2)
  this.manualMode = 1;
  m = getnominaltickcount(this);
  delt = (t2-t1)/m;
  n = ceil(log10(delt));
  rdelt = delt/10^n;
  p = floor(log2(rdelt));
  this.tDelt = 10^n*2^p;
  this.tStart = t1;
  this.tStop = t2;
  this.tBegin = this.tDelt*ceil(t1/this.tDelt);
  this.tEnd = floor(t2/this.tDelt)*this.tDelt;
  mprime = ceil((this.tEnd-this.tBegin)/this.tDelt);
  if ((this.tBegin+mprime*this.tDelt) > t2)
    mprime = mprime - 1;
  end
  this.tCount = mprime+1;
  setticks(this);
  
