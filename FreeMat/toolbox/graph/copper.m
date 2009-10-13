% COPPER COPPER Copper Colormap
% 
% Usage
% 
% Returns a copper colormap.  The syntax for its use is
% 
%    y = copper
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function map = copper
basecolor = [184,115,51]/255.0; basecolor = basecolor/basecolor(1);
map = linspace(0,1,256)'*basecolor;
