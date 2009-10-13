% COLORMAP COLORMAP Image Colormap Function
% 
% Usage
% 
% Changes the colormap for the current figure.  The generic syntax 
% for its use is
% 
%   colormap(map)
% 
% where map is a an array organized as 3 \times N),
% which defines the RGB (Red Green Blue) coordinates for each color in the
% colormap.  You can also use the function with no arguments to recover
% the current colormap
% 
%   map = colormap
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function cmap = colormap(A)
if (nargin == 0)
  cmap = get(gcf,'colormap');
else
  set(gcf,'colormap',A);
end
