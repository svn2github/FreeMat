%!
%@Module SURF Surface Plot Function
%@@Section HANDLE
%@@Usage
%This routine is used to create a surface plot of data.  A 
%surface plot is a 3D surface defined by the xyz coordinates
%of its vertices and optionally by the color at the vertices.
%The most general syntax for the @|surf| function is
%@[
%  h = surf(X,Y,Z,C,properties...)
%@]
%Where @|X| is a matrix or vector of @|x| coordinates, @|Y| is a
%matrix or vector of @|y| coordinates, @|Z| is a 2D matrix of
%coordinates, and @|C| is a 2D matrix of color values (the colormap
%for the current fig is applied).  In general, @|X| and @|Y| should
%be the same size as @|Z|, but FreeMat will expand vectors to match
%the matrix if possible.
%If you want the color of the surface to be defined by the height
%of the surface, you can omit @|C|
%@[
%  h = surf(X,Y,Z,properties...)
%@]
%in which case @|C=Z|.  You can also eliminate the @|X| and @|Y|
%matrices in the specification
%@[
%  h = surf(Z,properties)
%@]
%in which case they are set to @|1:size(Z,2)| and @|1:size(Y,2)|
%respectively.
%You can also specify a handle as the target of the @|surf| command
%via
%@[
%  h = surf(handle,...)
%@]
%@@Example
%Here we generate a surface specifying all four components.
%@<
%x = repmat(linspace(-1,1),[100,1]);
%y = x';
%r = x.^2+y.^2;
%z = exp(-r*3).*cos(5*r);
%c = r;
%surf(x,y,z,c)
%axis equal
%view(3)
%mprint surf1
%@>
%@figure surf1
%If we allow FreeMat to specify the color component, we see that
%the colorfield is the same as the height
%@<
%surf(x,y,z)
%mprint surf2
%@>
%@figure surf2
%!
function h = surf(varargin)
  % Check for an axes handle
  if (nargin>=2)
    if (isnumeric(varargin{1}) & (length(varargin{1})==1) & ...
    ishandle(varargin{1},'axes'))
       handle = varargin{1}(1);
       varargin(1) = [];
       nargin = nargin - 1;
    else   
       handle = newplot;
    end
  end
  % search for the propertyname/value pairs
  propstart = 0;
  if (nargin > 2)
    propstart = nargin-1;
    while ((propstart >= 1) & isa(varargin{propstart},'string') & ...
    pvalid('line',varargin{propstart}))
      propstart = propstart - 2;
    end
    propstart = propstart + 2;
  end
  propset = {};
  if ((propstart > 0) & (propstart < nargin))
     propset = varargin(propstart:end);
     varargin(propstart:end) = [];
  end
  if (length(varargin) == 0)
    h = surface(propset{:});
  elseif (length(varargin) == 1)
    h = surface('zdata',varargin{1},propset{:});
  elseif (length(varargin) == 3)
    h = surface('xdata',varargin{1},'ydata',varargin{2},...
	'zdata',varargin{3},propset{:});
  elseif (length(varargin) == 4)
    h = surface('xdata',varargin{1},'ydata',varargin{2},...
	'zdata',varargin{3},'cdata',varargin{4},propset{:});
  else
    error('Unrecognized arguments to surf command');
  end

