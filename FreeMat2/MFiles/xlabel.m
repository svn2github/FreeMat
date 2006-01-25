%!
%@Module XLABEL Plot X-axis Label Function
%@@Section HANDLE
%@@Usage
%This command adds a label to the x-axis of the plot.  The general syntax
%for its use is
%@[
%  xlabel('label')
%@]
%or in the alternate form
%@[
%  xlabel 'label'
%@]
%or simply
%@[
%  xlabel label
%@]
%Here @|label| is a string variable.  You can also specify properties
%for that label using the syntax
%@[
%  xlabel('label',properties...) 
%@]
%@@Example
%Here is an example of a simple plot with a label on the @|x|-axis.
%@<
%x = linspace(-1,1);
%y = cos(2*pi*x);
%plot(x,y,'r-');
%xlabel('time');
%mprint xlabel1
%@>
%which results in the following plot.
%@figure xlabel1
%!
function h = xlabel(varargin)
  if (nargin < 1) 
    error('xlabel needs at least one argument')
  end
  h = htext('string',varargin{1},'parent',gca,'horiz','center','vert','middle','autoparent','off',varargin{2:end});
  set(gca,'xlabel',h);
