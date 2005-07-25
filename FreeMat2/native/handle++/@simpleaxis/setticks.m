function setticks(&this)
  this.labelLocations = [];
  this.labels = {};
  exponentialForm = 0;
  for i=0:(this.tCount-1)
    tloc = this.tBegin+i*this.tDelt;
    this.labelLocations = [this.labelLocations,tloc];
    if (tloc ~= 0.0)
      exponentialForm = exponentialForm | (abs(log10(abs(tloc))) >= 4.0);
    end
  end
  for i=0:(this.tCount-1)
    this.labels{i+1} = trimprint(this.tBegin+i*this.tDelt,exponentialForm);
  end
  
