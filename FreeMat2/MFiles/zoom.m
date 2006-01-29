%!
%@Module ZOOM Image Zoom Function
%@@Section HANDLE
%@@Usage
%This function changes the zoom factor associated with the currently active
%image.  It is a legacy support function only, and thus is not quite equivalent
%to the @|zoom| function from previous versions of FreeMat.  However, it should
%achieve roughly the same effect. The generic syntax for its use is
%@[
%  zoom(x)
%@]
%where @|x| is the zoom factor to be used.  The exact behavior of the zoom
%factor is as follows:
%\begin{itemize}
%\item @|x>0| The image is zoomed by a factor @|x| in both directions.
%\item @|x=0| The image on display is zoomed to fit the size of the image window, but
%  the aspect ratio of the image is not changed.  (see the Examples section for
%more details).  This is the default zoom level for images displayed with the
%@|image| command.
%\item @|x<0| The image on display is zoomed to fit the size of the image window, with
%  the zoom factor in the row and column directions chosen to fill the entire window.
%  The aspect ratio of the image is not preserved.  The exact value of @|x| is
%  irrelevant.
%\end{itemize}
%@@Example
%To demonstrate the use of the @|zoom| function, we create a rectangular image 
%of a Gaussian pulse.  We start with a display of the image using the @|image|
%command, and a zoom of 1.
%@<
%x = linspace(-1,1,300)'*ones(1,600);
%y = ones(300,1)*linspace(-1,1,600);
%Z = exp(-(x.^2+y.^2)/0.3);
%image(Z);
%zoom(1.0);
%mprint zoom1
%@>
%@figure zoom1
%
%At this point, resizing the window accomplishes nothing, as with a zoom factor 
%greater than zero, the size of the image is fixed.
%
%If we change the zoom to another factor larger than 1, we enlarge the image by
%the specified factor (or shrink it, for zoom factors @|0 < x < 1|.  Here is the
%same image zoomed out to 60%
%@<
%image(Z);
%zoom(0.6);
%mprint zoom3
%@>
%@figure zoom3
%
%Similarly, we can enlarge it to 130%
%@<
%image(Z)
%zoom(1.3);
%mprint zoom4
%@>
%@figure zoom4
%
%The ``free'' zoom of @|x = 0| results in the image being zoomed to fit the window
%without changing the aspect ratio.  The image is zoomed as much as possible in
%one direction.
%@<
%image(Z);
%zoom(0);
%sizefig(200,400);
%mprint zoom5
%@>
%@figure zoom5
%
%The case of a negative zoom @|x < 0| results in the image being scaled arbitrarily.
%This allows the image aspect ratio to be changed, as in the following example.
%@<
%image(Z);
%zoom(-1);
%sizefig(200,400);
%mprint zoom6
%@>
%@figure zoom6
%!
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
set(gcf,'figsize',[new_width,new_height]);

function handle = get_image_child(ax)
children = get(ax,'children');
handle = [];
for i=1:numel(children)
  if (ishandle(children(i),'image'))
    handle = children(i);
    return;
  end
end

