%!
%@Module TITLE Plot Title Function
%@@Section HANDLE
%@@Usage
%This command adds a title to the plot.  The general syntax
%for its use is
%@[
%  title('label')
%@]
%or in the alternate form
%@[
%  title 'label'
%@]
%or simply
%@[
%  title label
%@]
%Here @|label| is a string variable.  You can also specify 
%properties for the label, and a handle to serve as a target
%for the operation
%@[
%  title(handle,'label',properties...)
%@]
%@@Example
%Here is an example of a simple plot with a title.
%@<
%x = linspace(-1,1);
%y = cos(2*pi*x);
%plot(x,y,'r-');
%title('cost over time');
%mprint title1
%@>
%which results in the following plot.
%@figure title1
%We now increase the size of the font using the properties
%of the @|label|
%@<
%title('cost over time','fontsize',20);
%mprint title2
%@>
%@figure title2
%!
function o = title(varargin)
  % Check for an axes handle
  if (nargin>=2)
    if (isnumeric(varargin{1}) & (length(varargin{1})==1) & ...
    ishandle(varargin{1},'axes'))
       handle = varargin{1}(1);
       varargin(1) = [];
       nargin = nargin - 1;
    else   
       handle = gca;
    end
  else
   handle = gca;
  end
  saveca = gca;
  axes(handle);
  if (length(varargin) == 0)
    error('must specify title text');
  end
  mtext = varargin{1};
  varargin(1) = [];
  nargin = nargin - 1;
  o = htext('string',mtext,'horiz','center','vert','top','position',[0.5,1,0],'parent',handle,'autoparent','off',varargin{:});
  set(handle,'title',o);
  axes(saveca);