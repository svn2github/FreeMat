%!
%@Module CLIM Adjust Color limits of plot
%@@Section HANDLE
%@@Usage
%There are several ways to use @|clim| to adjust the color limits of
%a plot.  The various syntaxes are
%@[
%   clim
%   clim([lo,hi])   
%   clim('auto')
%   clim('manual')
%   clim('mode')
%   clim(handle,...)
%@]
%The first form (without arguments), returns a 2-vector containing the
%current limits.  The second form sets the limits on the plot to @|[lo,hi]|.
%The third and fourth form set the mode for the limit to @|auto| and @|manual|
%respectively.  In @|auto| mode, FreeMat chooses the range for the axis 
%automatically.  The @|clim('mode')| form returns the current mode for the axis
%(either @|'auto'| or @|'manual'|).  
%
%Switching to @|manual| mode does not change the limits, it simply allows
% you to modify them (and disables the automatic adjustment of the limits
%as more objects are added to the plot).  Also, if you specify a set of 
%limits explicitly, the mode is set to @|manual|
% 
%Finally, you can specify the handle of an
%axis to manipulate instead of using the current one.
%@@Example
%Here is an example of using @|clim| to change the effective window and
%level onto an image.  First, the image with default
%limits
%@<
%x = repmat(linspace(-1,1),[100,1]); y = x';
%z = exp(-x.^2-y.^2);
%image(z);
%min(z(:))
%max(z(:))
%mprint clim1
%@>
%which results in
%@figure clim1
%Next, we change the colorscale of the image using the
% @|clim| function
%@<
%image(z);
%clim([0,0.2]);
%mprint clim2
%@>
%which results in
%@figure clim2
%!

%Copyright (c) 2004,2006 Brian Yanoff, Samit Basu
function ret = clim(varargin)
  if ((nargin > 0) && isnumeric(varargin{1}) && isscalar(varargin{1}) && ishandle(varargin{1}))
    handle = varargin{1};
    varargin(1) = [];
    nargin = nargin - 1;
  else
    handle = gca;
  end
  if (nargin == 0)
    ret = get(handle,'clim');
    return;
  end
  if (isstr(varargin{1}))
    if (strcmp(varargin{1},'mode'))
      ret = get(handle,'climmode');
      return;
    end
    if (strcmp(varargin{1},'auto'))
      set(handle,'climmode','auto');
      return;
    end
    if (strcmp(varargin{1},'manual'))
      set(handle,'climmode','manual');
      return;
    end
    error('Unrecognized argument to clim');
  elseif (isnumeric(varargin{1}) && numel(varargin{1})==2)
    set(gca,'clim',varargin{1});
  else
    error('Unrecognized argument to clim');
  end
