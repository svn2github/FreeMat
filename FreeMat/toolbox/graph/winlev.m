% WINLEV Image Window-Level Function
% 
% Usage
% 
% Adjusts the data range used to map the current image to the current
% colormap.  The general syntax for its use is
% 
%   winlev(window,level)
% 
% where window is the new window, and level is the new level, or
% 
%   winlev
% 
% in which case it returns a vector containing the current window
% and level for the active image.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function [win,lev] = winlev(window,level)
g = gca;
if (nargin == 2)
  set(g,'clim',[level-window/2,level+window/2]);
else
  clim = get(g,'clim');
  win = clim(2)-clim(1);
  lev = (clim(2)+clim(1))/2;
end

  
