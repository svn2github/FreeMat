% IMAGE Image Display Function
% 
% Usage
% 
% The image command has the following general syntax
% 
%   handle = image(x,y,C,properties...)
% 
% where x is a two vector containing the x coordinates
% of the first and last pixels along a column, and y is a
% two vector containing the y coordinates of the first and
% last pixels along a row.  The matrix C constitutes the
% image data.  It must either be a scalar matrix, in which case
% the image is colormapped using the  colormap for the current
% figure.  If the matrix is M x N x 3, then C is intepreted
% as RGB data, and the image is not colormapped.  The properties
% argument is a set of property/value pairs that affect the
% final image.  You can also omit the x and y, 
% 
%   handle = image(C, properties...)
% 
% in which case they default to x = [1,size(C,2)] and 
% y = [1,size(C,1)].  Finally, you can use the image function
% with only formal arguments
% 
%   handle = image(properties...)
% 
% 
% To support legacy FreeMat code, you can also use the following
% form of image
% 
%   image(C, zoomfactor)
% 
% which is equivalent to image(C) with the axes removed so that
% the image takes up the full figure window, and the size of the
% figure window adjusted to achieve the desired zoom factor using the
% zoom command.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function ohandle = image(varargin)
  ax = newplot;
  if (length(varargin) == 0), return; end
  % check for formal syntax
  if (isstr(varargin{1}))
    handle = himage(varargin{:});
  elseif ((length(varargin) == 1) || ((length(varargin) > 1) && (isstr(varargin{2}))))
    C = varargin{1}; varargin(1) = [];
    cols = max(2,size(C,2));
    rows = max(2,size(C,1));
    handle = himage('cdata',C,'xdata',[1,cols],'ydata',[1,rows],varargin{:});
    axis(ax,'image');
  elseif (numel(varargin) == 2)
    C = varargin{1};
    cols = max(2,size(C,2));
    rows = max(2,size(C,1));
    handle = himage('cdata',C,'xdata',[1,cols],'ydata',[1,rows]);
    axis maximal;
    axis tight;
    zoom(varargin{2});
  elseif (length(varargin) >= 3)
    x = varargin{1};
    y = varargin{2};
    C = varargin{3};
    varargin(1:3) = [];
    handle = himage('xdata',x([1,end]),'ydata',y([1,end]),'cdata',C,varargin{:});
%    axis(ax,'image');
  else
    error('Unrecognized form of image command');
  end
  if (nargout > 0)
      ohandle = handle;
  end
