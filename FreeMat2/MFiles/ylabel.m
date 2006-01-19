%!
%@Module YLABEL Plot Y-axis Label Function
%@@Section PLOT
%@@Usage
%This command adds a label to the y-axis of the plot.  The general syntax
%for its use is
%@[
%  ylabel('label')
%@]
%or in the alternate form
%@[
%  ylabel 'label'
%@]
%or simply
%@[
%  ylabel label
%@]
%You can also specify properties for that label using the syntax
%@[
%  ylabel('label',properties...) 
%@]
%@@Example
%Here is an example of a simple plot with a label on the @|y|-axis.
%@<
%x = linspace(-1,1);
%y = cos(2*pi*x);
%plot(x,y,'r-');
%ylabel('cost');
%mprint ylabel1
%@>
%which results in the following plot.
%@figure ylabel1
%!
function h = ylabel(varargin)
  if (nargin < 1) 
    error('ylabel needs at least one argument')
  end
  h = htext('string',varargin{1},'parent',gca,'horiz','center','vert','middle','autoparent','off',varargin{2:end});
  set(gca,'ylabel',h);
