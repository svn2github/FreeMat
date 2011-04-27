% ZOOM Image Zoom Function
% 
% Usage
% 
% This function changes the zoom factor associated with the currently active
% image.  It is a legacy support function only, and thus is not quite equivalent
% to the zoom function from previous versions of FreeMat.  However, it should
% achieve roughly the same effect. The generic syntax for its use is
% 
%   zoom(x)
% 
% where x is the zoom factor to be used.  The exact behavior of the zoom
% factor is as follows:
%   -  x>0 The image is zoomed by a factor x in both directions.
% 
%   -  x=0 The image on display is zoomed to fit the size of the image window, but
%   the aspect ratio of the image is not changed.  (see the Examples section for
% more details).  This is the default zoom level for images displayed with the
% image command.
% 
%   -  x<0 The image on display is zoomed to fit the size of the image window, with
%   the zoom factor in the row and column directions chosen to fill the entire window.
%   The aspect ratio of the image is not preserved.  The exact value of x is
%   irrelevant.
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function zoom(factor)
% Get the current axis handle
ax = gca;
% It should contain an image object
imhan = get_image_child(ax);
if (isempty(imhan)) return; end;
% Get the size of the image objects Cdata
C = get(imhan,'cdata');
newsize = size(C);
if (factor > 0)
  newsize = newsize * factor;
elseif (factor == 0)
  axis image;
  return;
elseif (factor < 0)
  axis normal;
  return;
end
% Get the axis position vector
position = get(ax,'position');
new_width = newsize(2)/position(3);
new_height = newsize(1)/position(4);
sizefig(new_width,new_height);

function handle = get_image_child(ax)
children = get(ax,'children');
handle = [];
for i=1:numel(children)
  if (ishandle(children(i),'image'))
    handle = children(i);
    return;
  end
end

