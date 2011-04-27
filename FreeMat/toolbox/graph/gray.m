% GRAY Gray Colormap
% 
% Usage
% 
% Returns a gray colormap.  The syntax for its use is
% 
%    y = gray
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function map = gray(m)
r = linspace(0,1,256)';
map = [r,r,r];
