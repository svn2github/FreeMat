function this = axes(varargin)
  if (nargin == 0)
    this.ActivePositionProperty = 'outerposition';
    this.ALim = [];
    this.ALimMode = 'auto';
    this.AmbientLightColor = [];
    this.BeingDeleted = 'off';
    this.Box = 'off';
    this.BusyAction = 'queue';
    this.ButtonDownFcn = [];
    this.CameraPosition = [0,0,1];
    this.CameraPositionMode = 'auto';
    this.CameraTarget = [0,0,0];
    this.CameraTargetMode = 'auto';
    this.CameraUpVector = [0,1,0];
    this.CameraViewAngle = [];
    this.CameraViewAngleMode = 'auto';
    this.Children = [];
    this.CLim = [];
    this.CLimMode = 'auto';
    this.Clipping = 'on';
    this.Color = 'none';
    this.ColorOrder = [];
    this.CreateFcn = [];
    this.CurrentPoint = [];
    this.DataAspectRatio = [];
    this.DataAspectRatioMode = 'auto';
    this.DeleteFcn = [];
    this.DrawMode = 'normal';
    this.FontAngle = 'normal';
    this.FontName = '';
    this.FontSize = 12;
    this.FontUnits = 'points';
    this.FontWeight = 'normal';
    this.GridLineStyle = ':';
    this.HandleVisibility = 'on';
    this.HitTest = 'on';
    this.Interruptible = 'on';
    this.Layer = 'bottom';
    this.LineStyleOrder = '-';
    this.LineWidth = 0.5;
    this.MinorGridLineStyle = ':';
    this.NextPlot = 'replace';
    this.OuterPosition = [];
    this.Parent = [];
    this.PlotBoxAspectRatio = [];
    this.PlotBoxAspectRatioMode = 'auto';
    this.Position = [];
    this.Projection = 'orthographic';
    this.Selected = 'off';
    this.SelectionHighlight = 'on';
    this.Tag = '';
    this.TickDir = 'in';
    this.TickDirMode = 'auto';
    this.TickLength = [];
    this.TightInset = [];
    this.Title = [];
    this.Type = 'axes';
    this.UIContextMenu = [];
    this.Units = 'normalized';
    this.UserData = [];
    this.Visible = 'on';
    this.XAxisLocation = 'bottom';
    this.YAxisLocation = 'left';
    this.XColor = [];
    this.YColor = [];
    this.ZColor = [];
    this.XDir = 'normal';
    this.YDir = 'normal';
    this.ZDir = 'normal';
    this.XGrid = 'off';
    this.YGrid = 'off';
    this.ZGrid = 'off';
    this.XLabel = [];
    this.YLabel = [];
    this.ZLabel = [];
    this.XLim = [];
    this.YLim = [];
    this.ZLim = [];
    this.XLimMode = 'auto';
    this.YLimMode = 'auto';
    this.ZLimMode = 'auto';
    this.XMinorGrid = 'off';
    this.YMinorGrid = 'off';
    this.ZMinorGrid = 'off';
    this.XScale = 'linear';
    this.YScale = 'linear';
    this.ZScale = 'linear';
    this.XTick = [];
    this.YTick = [];
    this.ZTick = [];
    this.XTickLabel = [];
    this.YTickLabel = [];
    this.ZTickLabel = [];
    this.XTickMode = 'auto';
    this.YTickMode = 'auto';
    this.ZTickMode = 'auto';
    this.XTickLabelMode = 'auto';
    this.YTickLabelMode = 'auto';
    this.ZTickLabelMode = 'auto';
    this = class(this,'axes');
  elseif (nargin == 1)
    if (isa(varargin{1},'axes'))
      this = varargin{1};
    else
      error('unrecognized form of axes func');
    end
  else
    this = axes;
    set(this,varargin);
  end

	  
    
    