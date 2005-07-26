function this = plot2d(a)
  if (nargin == 0)
    this.textPositions = [];
    this.textLabels = {};
    this.data = {};
    this.xaxis = simpleaxis;
    this.yaxis = simpleaxis;
    this.xlabel = '';
    this.ylabel = '';
    this.gridFlag = 0;
    this.title = '';
    this.space = 10;
    this.ticlen = 5;
    this.holdFlag = 0;
    this.holdSave = 0;
    this.sze_textheight = 0;
    this.viewport = [0,0,1,1];
    this = class(this,'plot2d');
  elseif (strcmp(class(a),'plot2d'))
    this = a;
  else
    this = class(a,'plot2d');
  end
  
