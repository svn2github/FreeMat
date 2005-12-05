function h = xlabel(varargin)
  if (nargin == 1)
    h = text('string',varargin{1},'parent',gca,'horiz','center','vert','middle');
    set(gca,'xlabel',h);
  else
    % Check to see if the first argument is numeric
    if (isnumeric(varargin{1}))
       axes = varargin{1}(1);
        h = text('string',varargin{2},'parent',axes,'horiz','center','vert','middle',varargin{2:end});
       set(axes,'xlabel',h);
    else
        h = text('string',varargin{1},'parent',gca,'horiz','center','vert','middle',varargin{2:end});
    	set(gca,'xlabel',h);
    end
  end
