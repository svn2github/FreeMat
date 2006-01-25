
%@Module ZOOM Image Zoom Function
%@@Section HANDLE
%@@Usage
%This function changes the zoom factor associated with the currently active
%image.  The generic syntax for its use is
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
%mprintimage zoom1
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
%mprintimage zoom3
%@>
%@figure zoom3
%
%Similarly, we can enlarge it to 130%
%@<
%image(Z)
%zoom(1.3);
%mprintimage zoom4
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
%mprintimage zoom5
%@>
%@figure zoom5
%
%The case of a negative zoom @|x < 0| results in the image being scaled arbitrarily.
%This allows the image aspect ratio to be changed, as in the following example.
%@<
%image(Z);
%zoom(-1);
%sizefig(200,400);
%mprintimage zoom6
%@>
%@figure zoom6

