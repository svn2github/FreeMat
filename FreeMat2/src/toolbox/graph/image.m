%!
%@Module IMAGE Image Display Function
%@@Section HANDLE
%@@Usage
%The @|image| command has the following general syntax
%@[
%  handle = image(x,y,C,properties...)
%@]
%where @|x| is a two vector containing the @|x| coordinates
%of the first and last pixels along a column, and @|y| is a
%two vector containing the @|y| coordinates of the first and
%last pixels along a row.  The matrix @|C| constitutes the
%image data.  It must either be a scalar matrix, in which case
%the image is colormapped using the  @|colormap| for the current
%figure.  If the matrix is @|M x N x 3|, then @|C| is intepreted
%as RGB data, and the image is not colormapped.  The @|properties|
%argument is a set of @|property/value| pairs that affect the
%final image.  You can also omit the @|x| and @|y|, 
%@[
%  handle = image(C, properties...)
%@]
%in which case they default to @|x = [1,size(C,2)]| and 
%@|y = [1,size(C,1)]|.  Finally, you can use the @|image| function
%with only formal arguments
%@[
%  handle = image(properties...)
%@]
%
%To support legacy FreeMat code, you can also use the following
%form of @|image|
%@[
%  image(C, zoomfactor)
%@]
%which is equivalent to @|image(C)| with the axes removed so that
%the image takes up the full figure window, and the size of the
%figure window adjusted to achieve the desired zoom factor using the
%@|zoom| command.
%@@Example
%In this example, we create an image that is @|512 x 512| pixels
%square, and set the background to a noise pattern.  We set the central
%@|128 x 256| pixels to be white.
%@<
%x = rand(512);
%x((-64:63)+256,(-128:127)+256) = 1.0;
%figure
%image(x)
%colormap(gray)
%mprint image1
%@>
%
%The resulting image looks like:
%@figure image1
%Here is an example of an RGB image 
%@<
%t = linspace(0,1);
%red = t'*t;
%green = t'*(t.^2);
%blue = t'*(0*t+1);
%A(:,:,1) = red; 
%A(:,:,2) = green; 
%A(:,:,3) = blue;
%image(A);
%mprint image2
%@>
%The resulting image looks like:
%@figure image2
%@@Tests
%@{ test_image1.m
%    % test image of an empty argument
%function test_val = test_image1
%a = [];
%try
%  % If this causes a segfault, it won't be caught.
%  image(a);
%catch
%end
%close all;
%test_val = 1;
%
%@}
%!

% Copyright (c) 2002-2006 Samit Basu

function ohandle = image(varargin)
  ax = newplot;
  if (length(varargin) == 0), return; end
  % check for formal syntax
  if (isstr(varargin{1}))
    handle = himage(varargin{:});
  elseif ((length(varargin) == 1) | ((length(varargin) > 1) & (isstr(varargin{2}))))
    C = varargin{1}; varargin(1) = [];
    handle = himage('cdata',C,'xdata',[1,size(C,2)],'ydata',...
                    [1,size(C,1)],varargin{:});
    axis(ax,'image');
  elseif (numel(varargin) == 2)
    C = varargin{1};
    axis maximal;
    handle = himage('cdata',C,'xdata',[1,size(C,2)],'ydata',[1,size(C,1)]);
    zoom(varargin{2});
  elseif (length(varargin) >= 3)
    x = varargin{1};
    y = varargin{2};
    C = varargin{3};
    varargin(1:3) = [];
    handle = himage('xdata',x,'ydata',y,'cdata',C,varargin{:});
    axis(ax,'image');
  else
    error('Unrecognized form of image command');
  end
  if (nargout > 0)
      ohandle = handle;
  end
