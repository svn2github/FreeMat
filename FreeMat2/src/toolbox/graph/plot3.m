%!
%@Module PLOT3 Plot 3D Function
%@@Section HANDLE
%@@Usage
%This is the 3D plot command.  The general syntax for its use is
%@[
%  plot3(X,Y,Z,{linespec 1},X,Y,Z,{linespec 2},...,properties...)
%@]
%where @|X| @|Y| and @|Z| are the coordinates of the points on the
%3D line.  Note that in general, all three should be vectors.  If
%some or all of the quantities are matrices, then FreeMat will attempt
%to expand the vector arguments to the same size, and then generate
%multiple plots, one for each column of the matrices.  The linespec
%is optional, see @|plot| for details.  You can specify @|properties|
%for the generated line plots.  You can also specify a handle as an
%axes to target
%@[
%  plot3(handle,...)
%@]
%@@Example
%Here is a simple example of a 3D helix.
%@<
%t = linspace(0,5*pi,200);
%x = cos(t); y = sin(t); z = t;
%plot3(x,y,z);
%view(3);
%mprint plt3
%@>
%Shown here
%@figure plt3
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function hout = plot3(varargin)
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
  saveca = gca;
  axes(handle);
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
  h = [];
  while (~isempty(varargin))
    cs = ''; ms = ''; ps = '';
    if (length(varargin) < 3)
      error('plot3 requires triplets of x, y, z coordinates');
    end;
    if (length(varargin) == 3 | (length(varargin) > 3) & ~islinespec(varargin{4},cs,ms,ps))
      h = [h,plot_triplet(varargin{1},varargin{2},varargin{3},handle,propset)];
      varargin(1:3) = [];
    elseif ((length(varargin) >= 4) & islinespec(varargin{4},cs,ms,ps))
      h = [h,plot_triplet(varargin{1},varargin{2},varargin{3},handle,completeprops(cs,ms,ps,propset))];
      varargin(1:4) = [];
    end;
  end
axes(saveca);
if (nargout > 0)
   hout = h;
end
    
function h = plot_triplet(X,Y,Z,handle,lineprops)
    h = [];
    if ((isvector(X) | isvector(Y) | isvector(Z)) & (~isvector(X) | ~isvector(Y) | ~isvector(Z)))
      rows = max([size(X,1),size(Y,1),size(Z,1)]);
      cols = max([size(X,2),size(Y,2),size(Z,2)]);
      X = expandmat(X,rows,cols);
      Y = expandmat(Y,rows,cols);
      Z = expandmat(Z,rows,cols);
    end
    if (isvector(X)), X = X(:); end;
    if (isvector(Y)), Y = Y(:); end;
    if (isvector(Z)), Z = Z(:); end;
    for i=1:size(Z,2)
      h = [h,tplotvector(handle,X(:,i),Y(:,i),Z(:,i),lineprops)];
    end

function x = expandmat(a,rows,cols)
if (length(b) == rows)
  x = repmat(b(:),[1,cols]);
elseif (length(b) == cols)
  x = repmat(b(:)',[rows,1]);
else
  error('plot3(X,Y,Z) where one or more arguments are vectors requires the other arguments to have a matching dimension');
end

function k = tplotvector(handle,x,y,z,lineprops)
  set(handle,'color','w');
  ndx = length(get(handle,'children'))+1;
  % Get the colororder
  colororder = get(handle,'colororder');
  % select the row using a modulo
  ndxmod = uint32(mod(ndx-1,size(colororder,1))+1);
  k = hline('xdata',x,'ydata',y,'zdata',z,'color',colororder(ndxmod,:),lineprops{:});

