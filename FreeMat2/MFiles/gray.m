%  GRAY Gray Colormap
%  
%  
%  USAGE
%  
%  Returns a gray colormap.  The syntax for its use is
%  
%     y = gray
%  
%  
function map = gray
r = linspace(0,1,256)';
map = [r,r,r];
