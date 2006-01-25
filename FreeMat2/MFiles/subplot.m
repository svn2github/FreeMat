%!
%@Module SUBPLOT Subplot Function
%@@Section HANDLE
%@@Usage
%This function divides the current figure into a 2-dimensional
%grid, each of which can contain a plot of some kind.  The function
%has a number of syntaxes.  The first version 
%@[
%   subplot(row,col,num)
%@]
%which either activates subplot number @|num|, or 
%sets up a subplot grid of size @|row x col|, and then
%activates @|num|. You can also set up subplots that cover multiple
%grid elements
%@[
%   subplot(row,col,[vec])
%@]
%where @|vec| is a set of indexes covered by the new subplot.
%Finally, as a shortcut, you can specify a string with three
%components
%@[
%   subplot('mnp')
%@]
%or using the alternate notation
%@[
%   subplot mnp
%@]
%where @|m| is the number of rows, @|n| is the number of columns
%and @|p| is the index.
%@@Example
%Here is the use of @|subplot| to set up a @|2 x 2| grid of plots
%@<
%t = linspace(-pi,pi);
%subplot(2,2,1)
%plot(t,cos(t).*exp(-2*t));
%subplot(2,2,2);
%plot(t,cos(t*2).*exp(-2*t));
%subplot(2,2,3);
%plot(t,cos(t*3).*exp(-2*t));
%subplot(2,2,4);
%plot(t,cos(t*4).*exp(-2*t));
%mprint subplot1
%@>
%@figure subplot1
%Here we use the second form of @|subplot| to generate one subplot
%that is twice as large.
%@<
%t = linspace(-pi,pi);
%subplot(2,2,[1,2])
%plot(t,cos(t).*exp(-2*t));
%subplot(2,2,3);
%plot(t,cos(t*3).*exp(-2*t));
%subplot(2,2,4);
%plot(t,cos(t*4).*exp(-2*t));
%mprint subplot2
%@>
%@figure subplot2
%Note that the subplots can contain any handle graphics objects,
%not only simple plots.
%@<
%t=0:(2*pi/100):(2*pi);
%x=cos(t*2).*(2+sin(t*3)*.3);
%y=sin(t*2).*(2+sin(t*3)*.3);
%z=cos(t*3)*.3;
%subplot(2,2,1)
%plot(t,x);
%subplot(2,2,2);
%plot(t,y);
%subplot(2,2,3);
%plot(t,z);
%subplot(2,2,4);
%tubeplot(x,y,z,0.14*sin(t*5)+.29,t,10)
%axis equal
%view(3)
%mprint subplot3
%@>
%@figure subplot3
%!
function h = subplot(varargin)
  m = 1; n = 1; p = 1;
  if (nargin == 1)
    if (isa(varargin{1},'string'))
      str = varargin{1};
      if (length(str) ~= 3), error('Subplot with a string argument requires a string of the type mnp'); end
      m = str(1)-'0';
      n = str(2)-'0';
      p = str(3)-'0';
    else
      axes(varargin{1}(1));
      h = varargin{1}(1);
    end
  else
    if (nargin >= 1), m = int32(varargin{1}(1)); end
    if (nargin >= 2), n = int32(varargin{2}(1)); end
    if (nargin >= 3), p = int32(varargin{3}); p = p(:); end
  end
  row = n+1-int32(idiv(p-1,n)+1);
  col = int32(mod(p-1,n)+1);
  width = 1.0/n;
  height = 1.0/m;
  left = (col-1)*width;
  bottom = (row-1)*height;
  position = [left,bottom,left+width,bottom+height];
  % Get the envelope of the axis
  if (length(p) > 1)
    position = [min(position(:,1:2)),max(position(:,3:4))];
  end
  position = [position(1),position(2),position(3)-position(1),...
	      position(4)-position(2)];
  fig = gcf;
  children = get(fig,'children');
  found = 0;
  deletelist = [];
  for (i=1:length(children))
    if (ishandle(children(i),'axes'))
      outerpos = get(children(i),'outerposition');
      if (all(outerpos==position))
	axes(children(i));
	found = 1;
      elseif (intersects(outerpos,position))
	deletelist = [deletelist,i];
      end
    end
  end
  children(deletelist) = [];
  set(fig,'children',children);
  if (~found)
    h = axes('outerposition',position);
  end
  set(fig,'nextplot','add');

function b = intersects(rect1,rect2)
  nleft = max(rect1(1),rect2(1));
  nbottom = max(rect1(2),rect2(2));
  nright = min(rect1(1)+rect1(3),rect2(1)+rect2(3));
  ntop = min(rect1(2)+rect1(4),rect2(2)+rect2(4));
  if ((nright <= nleft) | (ntop <= nbottom))
    b = 0;
  else
    area = (nright-nleft)*(ntop-nbottom);
    b = area > .01;
  end
