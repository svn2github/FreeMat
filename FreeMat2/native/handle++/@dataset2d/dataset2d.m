function this = dataset2d(a)
  if (nargin == 0)
    this.x = [];
    this.y = [];
    this.color = '';
    this.symbol = '';
    this.line = '';
    this.symbolLength = 3;
    this = class(this,'dataset2d');
  elseif (strcmp(class(a),'dataset2d'))
    this = a;
  else
    this = class(a,'dataset2d');
  end

