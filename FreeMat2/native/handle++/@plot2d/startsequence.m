function startsequence(&this)
  if (~holdFlag)
    this.data = {};
  end
  this.holdSave = this.holdFlag;
  this.holdFlag = true;
