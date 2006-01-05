%
%Control the axis behavior.  There are several versions of the
%axis command based on what you would like the axis command to
%do.  The first versions set scalings for the current plot.
%The general syntax for its use is
%@[
%  axis([xmin xmax ymin ymax zmin zmax cmin cmax])
%@]
%which sets the limits in the X, Y, Z and color axes.  You can
%also set only the X, Y and Z axes:
%@[
%  axis([xmin xmax ymin ymax zmin zmax])
%@]
%or only the X and Y axes:
%@[
%  axis([xmin xmax ymin ymax])
%@]
%To retrieve the current axis limits, use the syntax
%@[
%  x = axis
%@]
%where @|x| is a 4-vector for 2D plots, and a 6-vector for
%3D plots.
%
%There are a number of axis options supported by FreeMat.
%The first version sets the axis limits to be automatically
%selected by FreeMat for each dimension.  This state is the
%default one for new axes created by FreeMat.
%@[
%  axis auto
%@]
%The next option sets all of the axis limits to @|manual|
%mode.  This state turns off automatic scaling of the axis
%based on the children of the current axis object.  
%@[
%  axis manual
%@]
%The next option sets the axis limits to fit tightly around
%the data.
%@[
%  axis tight
%@]
%The next option adjusts the axis limits and plotbox aspect
%ratio so that the axis fills the position rectangle.
%@[
%  axis fill
%@]
%The next option puts the axis in matrix mode.  This mode
%is equivalent to the standard mode, but with the vertical
%axis reversed.  Thus, the origin of the coordinate system
%is at the top left corner of the plot.  This mode makes
%plots of matrix elements look normal (i.e., an identity
%matrix goes from upper left to lower right).
%@[
%  axis ij
%@]
%The next option puts the axis in normal mode, with the origin
%at the lower left corner.
%@[
%  axis xy
%@]
%The next option sets the axis parameters (specifically the
%data aspect ratio) so that equal ticks on each axis represent
%equal length.  In this mode, spheres look spherical insteal of
%ellipsoidal.
%@[
%  axis equal
%@]
%The next option is the same as @|axis equal|, but sets the
%plot box to fit tightly around the data (so no background shows
%through).  It is the best option to use when displaying images.
%@[
%  axis image
%@]
%The next option makes the axis box square.
%@[
%  axis square
%@]
%The next option restores many of the normal characteristics of
%the axis.  In particular, it undoes the effects of @|square|
%@|image| and @|equal| modes.
%@[
%  axis normal
%@]
%The next mode freezes axis properties so that 3D objects can
%be rotated properly.
%@[
%  axis vis3d
%@]
%The next mode turns off all labels, tick marks and background.
%@[
%  axis on
%@]
%The next mode turns on all labels, tick marks and background.
%@[
%  axis off
%@]
%
%The @|axis| command can also be applied to a particular axis
%(as opposed to the current axis as returned by @|gca|) handle
%@[
%  axis(M,...)
%@]
function retvec = axis(varargin);
if (length(varargin) > 0)
  if (ishandle(varargin{1},'axes'))
    ax = varargin{1};
    varargin(1) = [];
  else
    ax = gca;
  end
else
  ax = gca;
end

if (length(varargin) == 0)
  % no args, just return the limits
  if (is2Dview(ax))
    retvec = [get(ax,'xlim'),get(ax,'ylim')];
  else 
    retvec = [get(ax,'xlim'),get(ax,'ylim'),get(ax,'zlim')];
  end
else
  mode = varargin{1};
  if (isnumeric(mode))
    HandleNumericMode(ax,mode);
  elseif (strncmp(mode,'auto',4))
    HandleAutoMode(ax,mode);
  elseif (strcmp(mode,'manual'))
    HandleManualMode(ax);
  elseif (strcmp(mode,'tight'))
    HandleTightMode(ax);
  elseif (strcmp(mode,'fill'))
    HandleFillMode(ax);
  elseif (strcmp(mode,'ij'))
    HandleIJMode(ax);
  elseif (strcmp(mode,'xy'))
    HandleXYMode(ax);
  elseif (strcmp(mode,'equal'))
    HandleEqualMode(ax);
  elseif (strcmp(mode,'image'))
    HandleImageMode(ax);
  elseif (strcmp(mode,'square'))
    HandleSquareMode(ax);
  elseif (strcmp(mode,'normal'))
    HandleNormalMode(ax);
  elseif (strcmp(mode,'vis3d'))
    HandleVis3DMode(ax);
  elseif (strcmp(mode,'off'))
    HandleOffMode(ax);
  elseif (strcmp(mode,'on'))
    HandleOnMode(ax);
  else
    error(['Unrecognized mode ' mode ' argument to axis command']);
  end
end

% axis autoxz --> xlimmod->auto, zlimmode->auto, ylimmode->manual
% axis manual --> xyzlimmode->manual
% axis tight  --> set tight limits
% axis fill   --> set plot box aspect so that axis fills position
% axis ij     --> xdir->normal, ydir->reverse
% axis xy     --> xdir->normal, ydir->normal
% axis equal  --> 3D plot, --> image,
%                 2D plot, dar->111, ?
% axis image  --> dar->111, pbar-->auto, tight
% axis square --> pbar->111, dar->auto
% axis normal --> pbar->auto, dar->auto
% axis vis3d  --> darmode->manual, pbarmode->manual
% axis off    --> visible off, title->visible on
% axis on     --> visible on

function HandleFillMode(ax)
  pos = get(ax,'position');
  fsize = get(get(ax,'parent'),'figsize');
  width = pos(3)*fsize(1); height = pos(3)*fsize(2);
  set(ax,'plotboxaspectratio',[width,height,1]);

function HandleOnMode(ax)
  set(ax,'visible','on');

function HandleOffMode(ax)
  set(ax,'visible','off');

function HandleVis3DMode(ax)
  set(ax,'plotboxaspectratiomode','manual','dataaspectratiomode','manual');

function HandleNormalMode(ax)
  set(ax,'plotboxaspectratiomode','auto','dataaspectratiomode','auto');

function HandleSquareMode(ax)
  set(ax,'plotboxaspectratio',[1,1,1],'dataaspectratiomode','auto');

function HandleImageMode(ax)
  set(ax,'dataaspectratio',[1,1,1],'plotboxaspectratiomode','auto');
  lims = get(ax,'datalimits');
  set(ax,'xlim',lims(1:2),'ylim',lims(3:4),'zlim',lims(5:6));

function HandleEqualMode(ax)
  set(ax,'dataaspectratio',[1,1,1]);

function HandleXYMode(ax)
  set(ax,'xdir','normal','ydir','normal');

function HandleIJMode(ax)
  set(ax,'xdir','normal','ydir','reverse');

function HandleTightMode(ax)
  lims = get(ax,'datalimits');
  set(ax,'xlim',lims(1:2),'ylim',lims(3:4),'zlim',lims(5:6));

function HandleManualMode(ax)
  set(ax,'xlimmode','manual','ylimmode','manual','zlimmode','manual');

function HandleAutoMode(ax,mode)
  allactive = strcmp(mode,'auto');
  xactive = any(find(mode == 'x'));
  yactive = any(find(mode == 'y'));
  zactive = any(find(mode == 'z'));
  if (allactive | xactive), set(ax,'xlimmode','auto'), end
  if (allactive | yactive), set(ax,'ylimmode','auto'), end
  if (allactive | zactive), set(ax,'zlimmode','auto'), end

function HandleNumericMode(ax,mode)
  mode = mode(:);
  if (length(mode) == 2)
    set(ax,'xlim',mode);
  elseif (length(mode) == 4)
    set(ax,'xlim',mode(1:2),'ylim',mode(3:4));
  elseif (length(mode) == 6)
    set(ax,'xlim',mode(1:2),'ylim',mode(3:4),'zlim',mode(5:6));
  elseif (length(mode) == 8)
    set(ax,'xlim',mode(1:2),'ylim',mode(3:4),'zlim',mode(5:6),'clim',mode(7:8));
  else
    error('Unrecognized mode in call to axis - expected 2,4,6 or 8 arguments');
  end
