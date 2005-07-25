function setaxislength(&this, npix)
  this.axisLength = npix;
  if (this.manualMode)
    manualsetaxis(this,this.tStart,this.tStop);
  else
    autosetaxis(this);
  end
  
