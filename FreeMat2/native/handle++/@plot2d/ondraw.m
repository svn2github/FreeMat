function ondraw(this)
  sze = getcanvassize;
  width = sze(1);
  height = sze(2);
  setforegroundcolor([211,211,211]);
  fillrectangle([0,0,sze(1),sze(2)]);
  setfontsize(12);
  client_y_offset = 5;
  client_x_offset = 5;
  width = width - 10;
  height = height - 10;
  t = gettextextent('|');
  this.sze_textheight - t(2);

  xlabels = getticklabels(this.xaxis);
  ylabels = getticklabels(this.yaxis);

  
  plotWidth = width - this.space - this.sze_textheight;
  plotX = this.space + this.sze_textheight;
  if (~isempty(this.ylabel))
     plotWidth = plotWidth - (this.space+this.sze_textheight);
     plotX = plotX + this.space + this.sze_textheight;
  end

  t = gettextextent(xlabels{end});
  plotWidth = plotWidth - t(1)/2;

  maxwidth = 0;
  for i=1:length(ylabels)
    t = gettextextent(ylabels{i});
    maxwidth = max(maxwidth,t(1));
  end
  plotX = plotX + maxwidth + this.space/2;
  plotWidth = plotWidth - (maxwidth+this.space/2);

  plotHeight = height-2*this.space-this.sze_textheight;
  plotY = this.space;

  if (~isempty(this.xlabel))
     plotHeight = plotHeight - (this.space+this.sze_textheight);
  end

  if (~isempty(this.title))
     plotHeight = plotHeight - (this.space+this.sze_textheight);
     plotY = plotY + this.space + this.sze_textheight;
  end

  setforegroundcolor([0,0,0]);
  if (~isempty(this.title))
     drawtextstringaligned(title,[plotX+plotWidth/2,this.space],...
				'center','top',0);
  end  

  this.viewport = [plotX, plotY, plotWidth + 1, plotHeight + 1];
  setforegroundcolor([255,255,255]);
  fillrectangle(this.viewport);
  setaxislength(this.xaxis,plotWidth);
  setaxislength(this.yaxis,plotHeight);

  drawaxes(this);
  pushclip(this.viewport);
  for i=1:length(data)
    drawme(data{i},this);
  end
  popclip;