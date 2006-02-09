%!
%@Module VIEW Set Graphical View
%@@Section HANDLE
%@@Usage
%The @|view| function sets the view into the current plot.
%The simplest form is
%@[
%  view(n)
%@]
%where @|n=2| sets a standard view (azimuth 0 and elevation 90),
%and @|n=3| sets a standard 3D view (azimuth 37.5 and elevation 30).
%With two arguments,
%@[
%  view(az,el)
%@]
%you set the viewpoint to azimuth @|az| and elevation @|el|.
%@@Example
%Here is a 3D surface plot shown with a number of viewpoints.
%First, the default view for a 3D plot.
%@<
%x = repmat(linspace(-1,1),[100,1]);
%y = x';
%r = x.^2+y.^2;
%z = exp(-r*3).*cos(5*pi*r);
%surf(x,y,z);
%axis equal
%view(3)
%mprint view1
%@>
%@figure view1
%Next, we look at it as a 2D plot
%@<
%surf(x,y,z);
%axis equal
%view(2)
%mprint view2
%@>
%@figure view2
%Finally, we generate a different view of the same surface.
%@<
%surf(x,y,z);
%axis equal
%view(25,50);
%mprint view3
%@>
%@figure view3
%!

% Copyright (c) 2002-2006 Samit Basu

function view(varargin)
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
  % Calculate the radius of the cube
  xlim = get(gca,'xlim'); xmean = mean(xlim);
  ylim = get(gca,'ylim'); ymean = mean(ylim);
  zlim = get(gca,'zlim'); zmean = mean(zlim);
  xmax = max(abs(xlim-xmean));
  ymax = max(abs(ylim-ymean));
  zmax = max(abs(zlim-zmean));
  r = sqrt(xmax^2+ymax^2+zmax^2);
  z = sin(el)*r + zmean;
  y = -cos(el)*cos(az)*r + ymean;
  x = cos(el)*sin(az)*r + xmean;
  set(gca,'cameraposition',[x,y,z],'cameraupvector',[0,0,1]);
