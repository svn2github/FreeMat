function T = view(varargin)
  if (nargin == 0)
    error('Expected an argument to view function');
  end;
  if (nargin == 1)
    arg = varargin{1};
    if (length(arg) == 1)
      if (arg == 2)
	view(0,90);
	return;
      elseif (arg == 3)
	view(37.5,30);
	return;
      else
	error('Unrecognized form of view');
      end
    end
  else
    az = varargin{1};
    el = varargin{2};
  end
  az = az*pi/180;
  el = el*pi/180;
  % The view point is first calculated by transforming to
  % spherical coordinates
  z = sin(el);
  y = -cos(el)*cos(az);
  x = cos(el)*sin(az);
  set(gca,'cameraposition',[x,y,z],'cameraupvector',[0,0,1]);
  T = [];
