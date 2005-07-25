function addtext(&this, x, y, val)
  this.textPositions = [this.textPositions;x,y];
  this.textLabels = [this.textLabels,{val}];
