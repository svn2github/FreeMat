%!
%@Module WINLEV Image Window-Level Function
%@@Section HANDLE
%@@Usage
%Adjusts the data range used to map the current image to the current
%colormap.  The general syntax for its use is
%@[
%  winlev(window,level)
%@]
%where @|window| is the new window, and @|level| is the new level, or
%@[
%  winlev
%@]
%in which case it returns a vector containing the current window
%and level for the active image.
%@@Function Internals
%FreeMat deals with scalar images on the range of @|[0,1]|, and must
%therefor map an arbitrary image @|x| to this range before it can
%be displayed.  By default, the @|image| command chooses 
%\[
%  \mathrm{window} = \max x - \min x,
%\]
%and
%\[
%  \mathrm{level} = \frac{\mathrm{window}}{2}
%\]
%This ensures that the entire range of image values in @|x| are 
%mapped to the screen.  With the @|winlev| function, you can change
%the range of values mapped.  In general, before display, a pixel @|x|
%is mapped to @|[0,1]| via:
%\[
%   \max\left(0,\min\left(1,\frac{x - \mathrm{level}}{\mathrm{window}}
%   \right)\right)
%\]
%@@Examples
%The window level function is fairly easy to demonstrate.  Consider
%the following image, which is a Gaussian pulse image that is very 
%narrow:
%@<
%t = linspace(-1,1,256);
%xmat = ones(256,1)*t; ymat = xmat';
%A = exp(-(xmat.^2 + ymat.^2)*100);
%image(A);
%mprint('winlev1');
%@>
%The data range of @|A| is @|[0,1]|, as we can verify numerically:
%@<
%min(A(:))
%max(A(:))
%@>
%To see the tail behavior, we use the @|winlev| command to force FreeMat
%to map a smaller range of @|A| to the colormap.
%@<
%image(A);
%winlev(1e-4,0.5e-4)
%mprint('winlev2');
%@>
%The result is a look at more of the tail behavior of @|A|.
%We can also use the winlev function to find out what the
%window and level are once set, as in the following example.
%@<
%image(A);
%winlev(1e-4,0.5e-4)
%winlev
%@>
%!
function winlev(window,level)
g = gca;
set(g,'clim',[level-window/2,level+window/2]);
