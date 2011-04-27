% YLIM Adjust Y Axis limits of plot
% 
% Usage
% 
% There are several ways to use ylim to adjust the Y axis limits of
% a plot.  The various syntaxes are
% 
%    ylim
%    ylim([lo,hi])   
%    ylim('auto')
%    ylim('manual')
%    ylim('mode')
%    ylim(handle,...)
% 
% The first form (without arguments), returns a 2-vector containing the
% current limits.  The second form sets the limits on the plot to [lo,hi].
% The third and fourth form set the mode for the limit to auto and manual
% respectively.  In auto mode, FreeMat chooses the range for the axis 
% automatically.  The ylim('mode') form returns the current mode for the axis
% (either 'auto' or 'manual').  Finally, you can specify the handle of an
% axis to manipulate instead of using the current one.
% 
% As an additional feature, you can now specify inf for a limit, and
% FreeMat will take that limit from the automatic set.  So, for example 
% ylim([10,inf]) will set the minimum for the y axis, but use the
% automatic value for the maximum.

%Copyright (c) 2004,2006 Brian Yanoff, Samit Basu
% Licensed under the GPL
function ret = ylim(varargin)
  if ((nargin > 0) && isnumeric(varargin{1}) && isscalar(varargin{1}) && ishandle(varargin{1}))
    handle = varargin{1};
    varargin(1) = [];
    nargin = nargin - 1;
  else
    handle = gca;
  end
  if (nargin == 0)
    ret = get(handle,'ylim');
    return;
  end
  if (isstr(varargin{1}))
    if (strcmp(varargin{1},'mode'))
      ret = get(handle,'ylimmode');
      return;
    end
    if (strcmp(varargin{1},'auto'))
      set(handle,'ylimmode','auto');
      return;
    end
    if (strcmp(varargin{1},'manual'))
      set(handle,'ylimmode','manual');
      return;
    end
    error('Unrecognized argument to ylim');
  elseif (isnumeric(varargin{1}) && numel(varargin{1})==2)
    set(gca,'ylim',varargin{1});
  else
    error('Unrecognized argument to ylim');
  end
