%!
%@Module COPPER Copper Colormap
%@@Section HANDLE
%@@Usage
%Returns a copper colormap.  The syntax for its use is
%@[
%   y = copper
%@]
%@@Example
%Here is an example of an image displayed with the @|copper|
%colormap
%@<
%x = linspace(-1,1,512)'*ones(1,512);
%y = x';
%Z = exp(-(x.^2+y.^2)/0.3);
%image(Z);
%colormap(copper);
%mprint copper1
%@>
%which results in the following image
%@figure copper1
%!
function map = copper
basecolor = [184,115,51]/255.0; basecolor = basecolor/basecolor(1);
map = linspace(0,1,256)'*basecolor;
