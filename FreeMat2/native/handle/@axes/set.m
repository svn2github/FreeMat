function set(&this,varargin)
  if (length(varargin) > 2)
    p = int32(floor(length(varargin)/2));
    for i=1:p
      set(this,varargin{2*i-1},varargin{2*i});
    end
  else
    name = varargin{1};
    prop = varargin{2};
    fields = fieldnames(this);
    match_up = find(strncmp(fields,name,length(name)));
    match_count = length(match_up);
    if (match_count == 0)
      error(sprintf('Unable to match property name %s to axes',name));
    elseif (match_count > 1)
      error(sprintf('Ambiguous property name %s for axes',name));
    else
      name = fields{match_up};
    end
    SetCheck(this,name,'ActivePositionProperty',@setchoice,prop,{'outerposition','position'});
    SetCheck(this,name,'ALim',@setvector,prop,2);
    SetCheck(this,name,'ALimMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'AmbientLightColor',@setcolor,prop);
    SetCheck(this,name,'BeingDeleted',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'Box',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'BusyAction',@setchoice,prop,{'cancel','queue'});
%    SetCheck(this,name,'ButtonDownFcn',@setfcn,prop);
    SetCheck(this,name,'CameraPosition',@setvector,prop,3);
    SetCheck(this,name,'CameraPositionMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'CameraTarget',@setvector,prop,3);
    SetCheck(this,name,'CameraTargetMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'CameraUpVector',@setvector,prop,3);
    SetCheck(this,name,'CameraViewAngle',@setvector,prop,1);
    SetCheck(this,name,'CameraViewAngleMode',@setchoice,prop,{'auto','manual'});
%    SetCheck(this,name,'Children',@sethandles,prop,inf);
    SetCheck(this,name,'CLim',@setvector,prop,2);
    SetCheck(this,name,'CLimMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'Color',@setcolor,prop);
 %   SetCheck(this,name,'ColorOrder',@setmatrix,prop,inf,3);
 %   SetCheck(this,name,'CreateFcn',@setfcn,prop);
    SetCheck(this,name,'DataAspectRatio',@setvector,prop,3);
    SetCheck(this,name,'DataAspectRatioMode',@setchoice,prop,{'auto','manual'});
 %   SetCheck(this,name,'DeleteFcn',@setfcn,prop);
    SetCheck(this,name,'DrawMode',@setchoice,prop,{'normal','fast'});
    SetCheck(this,name,'FontAngle',@setchoice,prop,{'normal','italic','oblique'});
 %   SetCheck(this,name,'FontName',@setfontname,prop);
    SetCheck(this,name,'FontSize',@setvector,prop,1);
    SetCheck(this,name,'FontUnits',@setchoice,prop,{'points','normalized','inches','centimeters','pixels'});
    SetCheck(this,name,'FontWeight',@setchoice,prop,{'normal','bold','light','demi'});;
    SetCheck(this,name,'GridLineStyle',@setchoice,prop,{'-','--',':','-.','none'});
    SetCheck(this,name,'HandleVisibility',@setchoice,prop,{'on','callback','off'});
    SetCheck(this,name,'HitTest',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'Interruptible',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'Layer',@setchoice,prop,{'bottom','top'});
%    SetCheck(this,name,'LineStyleOrder',@setlinestylelist,prop);
    SetCheck(this,name,'LineWidth',@setvector,prop,1);
    SetCheck(this,name,'MinorGridLineStyle',@setchoice,prop,{'-','--',':','-.','none'});
    SetCheck(this,name,'NextPlot',@setchoice,prop,{'add','replace','replacechildren'});
    SetCheck(this,name,'OuterPosition',@setvector,prop,4);
%    SetCheck(this,name,'Parent',@sethandles,prop,1);
    SetCheck(this,name,'PlotBoxAspectRatio',@setvector,prop,3);
    SetCheck(this,name,'PlotBoxAspectRatioMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'Position',@setvector,prop,4);
    SetCheck(this,name,'Projection',@setchoice,prop,{'orthographic','perspective'});
    SetCheck(this,name,'Selected',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'SelectionHightlight',@setchoice,prop,{'on','off'});
%    SetCheck(this,name,'Tag',@setstrings,prop,1);
    SetCheck(this,name,'TickDir',@setchoice,prop,{'in','out'});
    SetCheck(this,name,'TickDirMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'TickLength',@setvector,prop,2);
 %   SetCheck(this,name,'Title',@sethandles,prop,1);
    SetCheck(this,name,'Units',@setchoice,prop,{'inches','centimeters','normalized','points','pixels','characters'});
    SetCheck(this,name,'UserData',@setany,prop);
    SetCheck(this,name,'Visible',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'XAxisLocation',@setchoice,prop,{'top','bottom'});
    SetCheck(this,name,'YAxisLocation',@setchoice,prop,{'right','left'});
    SetCheck(this,name,'XColor',@setcolor,prop);
    SetCheck(this,name,'YColor',@setcolor,prop);
    SetCheck(this,name,'ZColor',@setcolor,prop);
    SetCheck(this,name,'XGrid',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'YGrid',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'ZGrid',@setchoice,prop,{'on','off'});
%    SetCheck(this,name,'XLabel',@sethandles,prop,1);
%    SetCheck(this,name,'YLabel',@sethandles,prop,1);
%    SetCheck(this,name,'ZLabel',@sethandles,prop,1);
    SetCheck(this,name,'XLim',@setvector,prop,2);
    SetCheck(this,name,'YLim',@setvector,prop,2);
    SetCheck(this,name,'ZLim',@setvector,prop,2);
    SetCheck(this,name,'XLimMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'YLimMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'ZLimMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'XMinorGrid',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'YMinorGrid',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'ZMinorGrid',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'XMinorTicks',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'YMinorTicks',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'ZMinorTicks',@setchoice,prop,{'on','off'});
    SetCheck(this,name,'XScale',@setchoice,prop,{'linear','log'});
    SetCheck(this,name,'YScale',@setchoice,prop,{'linear','log'});
    SetCheck(this,name,'ZScale',@setchoice,prop,{'linear','log'});
    SetCheck(this,name,'XTick',@setvector,prop,inf);
    SetCheck(this,name,'YTick',@setvector,prop,inf);
    SetCheck(this,name,'ZTick',@setvector,prop,inf);
%    SetCheck(this,name,'XTickLabel',@setstrings,prop,inf);
%    SetCheck(this,name,'YTickLabel',@setstrings,prop,inf);
%    SetCheck(this,name,'ZTickLabel',@setstrings,prop,inf);
    SetCheck(this,name,'XTickMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'YTickMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'ZTickMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'XTickLabelMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'YTickLabelMode',@setchoice,prop,{'auto','manual'});
    SetCheck(this,name,'ZTickLabelMode',@setchoice,prop,{'auto','manual'});
  end

function SetCheck(this,name,property,handler,data,varargin)
  if (strcmp(name,property))
    this.(name) = feval(handler,name,data,varargin{:});
  end

function select = setchoice(name,prop,selections)
  if (length(find(strcmp(selections,prop))) == 0)
    error(sprintf('illegal value of %s for property %s',prop,name));
  end
  select = prop;

function select = setany(name,prop)
  select = prop;

function select = setvector(name,prop,len)
  if (isinf(len) | prod(size(prop)) == len)
    select = prop(:)';
  else
    error(sprintf('expecting a length %d vector for property %s',len,name));
  end;
 
function select = setcolor(name,prop)
  if (ischar(prop))
    if (any(strcmp({'y','yellow'},prop)))
      select = [1,1,0];
    elseif (any(strcmp({'m','magenta'},prop)))
      select = [1,0,1];
    elseif (any(strcmp({'c','cyan'},prop)))
      select = [0,1,1];
    elseif (any(strcmp({'r','red'},prop)))
      select = [1,0,0];
    elseif (any(strcmp({'g','green'},prop)))
      select = [0,1,0];
    elseif (any(strcmp({'b','blue'},prop)))
      select = [0,0,1];
    elseif (any(strcmp({'w','white'},prop)))
      select = [1,1,1];
    elseif (any(strcmp({'k','black'},prop)))
      select = [0,0,0];
    elseif (strcmp(prop,'none'));
      select = [];
    end
  else
    if (length(prop) ~= 3 | min(prop(:)) < 0 | max(prop(:)) > 1)
      error('expected RGB spec for property %s as a three vector in the range of 0,1',name);
    end;
    select = prop(:)';
  end;