% ZLIM ZLIM Adjust Z Axis limits of plot
% 
% Usage
% 
% There are several ways to use zlim to adjust the Z axis limits of
% a plot.  The various syntaxes are
% 
%    zlim
%    zlim([lo,hi])   
%    zlim('auto')
%    zlim('manual')
%    zlim('mode')
%    zlim(handle,...)
% 
% The first form (without arguments), returns a 2-vector containing the
% current limits.  The second form sets the limits on the plot to [lo,hi].
% The third and fourth form set the mode for the limit to auto and manual
% respectively.  In auto mode, FreeMat chooses the range for the axis 
% automatically.  The zlim('mode') form returns the current mode for the axis
% (either 'auto' or 'manual').  Finally, you can specify the handle of an
% axis to manipulate instead of using the current one.

%Copyright (c) 2004,2006 Brian Yanoff, Samit Basu
% Licensed under the GPL
function ret = zlim(varargin)
  if ((nargin > 0) && isnumeric(varargin{1}) && isscalar(varargin{1}) && ishandle(varargin{1}))
    handle = varargin{1};
    varargin(1) = [];
    nargin = nargin - 1;
  else
    handle = gca;
  end
  if (nargin == 0)
    ret = get(handle,'zlim');
    return;
  end
  if (isstr(varargin{1}))
    if (strcmp(varargin{1},'mode'))
      ret = get(handle,'zlimmode');
      return;
    end
    if (strcmp(varargin{1},'auto'))
      set(handle,'zlimmode','auto');
      return;
    end
    if (strcmp(varargin{1},'manual'))
      set(handle,'zlimmode','manual');
      return;
    end
    error('Unrecognized argument to zlim');
  elseif (isnumeric(varargin{1}) && numel(varargin{1})==2)
    set(gca,'zlim',varargin{1});
  else
    error('Unrecognized argument to zlim');
  end
