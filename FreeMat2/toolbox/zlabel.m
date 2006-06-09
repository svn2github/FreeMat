%!
%@Module ZLABEL Plot Z-axis Label Function
%@@Section HANDLE
%@@Usage
%This command adds a label to the z-axis of the plot.  The general syntax
%for its use is
%@[
%  zlabel('label')
%@]
%or in the alternate form
%@[
%  zlabel 'label'
%@]
%or simply
%@[
%  zlabel label
%@]
%Here @|label| is a string variable.  You can also specify properties
%for that label using the syntax
%@[
%  zlabel('label',properties...) 
%@]
%@@Example
%Here is an example of a simple plot with a label on the @|z|-axis.
%@<
%t = linspace(0,5*pi);
%x = cos(t);
%y = sin(t);
%z = t;
%plot3(x,y,z,'r-');
%view(3);
%zlabel('time');
%mprint zlabel1
%@>
%which results in the following plot.
%@figure zlabel1
%!

% Copyright (c) 2002-2006 Samit Basu

function h = zlabel(varargin)
  if (nargin < 1) 
    error('zlabel needs at least one argument')
  end
  h = htext('string',varargin{1},'parent',gca,'horiz','center','vert','middle','autoparent','off',varargin{2:end});
  set(gca,'zlabel',h);
