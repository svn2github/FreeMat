function addplot(&this,dataset)
  if (~this.holdFlag)
    this.data = {};
  end
  this.data = [this.data,{dataset}];
  setaxesauto(this);
