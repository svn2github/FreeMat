function handle = image(varargin)
  ax = newplot;
  if (length(varargin) == 0), return; end
  % check for formal syntax
  if (isstr(varargin{1}))
    handle = himage(varargin{:});
  elseif ((length(varargin) == 1) | ((length(varargin) > 1) & (isstr(varargin{2}))))
    C = varargin{1}; varargin(1) = [];
    handle = himage('cdata',C,'xdata',[1,size(C,2)],'ydata',...
                    [1,size(C,1)],varargin{:});
    axis(ax,'image');
  elseif (length(varargin) >= 3)
    x = varargin{1};
    y = varargin{2};
    C = varargin{3};
    varargin(1:3) = [];
    handle = himage('xdata',x,'ydata',y,'cdata',C,varargin{:});
    axis(ax,'image');
  else
    error('Unrecognized form of image command');
  end

