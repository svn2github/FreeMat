%!
%@Module TEXT Add Text Label to Plot
%@@Section HANDLE
%@@Usage
%Adds a text label to the currently active plot.  The general
%syntax for it is use is either
%@[
%   text(x,y,'label')
%@]
%where @|x| and @|y| are both vectors of the same length, in which
%case the text @|'label'| is added to the current plot at each of the
%coordinates @|x(i),y(i)| (using the current axis to map these to screen
%coordinates).  The second form supplies a cell-array of strings
%as the second argument, and allows you to place many labels simultaneously
%@[
%   text(x,y,{'label1','label2',....})
%@]
%where the number of elements in the cell array must match the size of
%vectors @|x| and @|y|.  You can also specify properties for the labels
%via
%@[
%   handles = text(x,y,{labels},properties...)
%@]
%@@Example
%Here is an example of a few labels being added to a random plot:
%@<
%plot(rand(1,4))
%text([2,3],[0.5,0.5],{'hello','there'})
%mprint text1
%@>
%@figure text1
%Here is the same example, but with larger labels:
%@<
%plot(rand(1,4))
%text([2,3],[0.5,0.5],{'hello','there'},'fontsize',20)
%mprint text2
%@>
%@figure text2
%!

% Copyright (c) 2002-2006 Samit Basu

function text(varargin)
if (nargin < 3)
  error 'text requires at least three arguments, the x and y location vectors and the strings'
end
xvec = varargin{1};
yvec = varargin{2};
labels = varargin{3};
varargin(1:3) = [];
if (length(xvec) ~= length(yvec))
  error 'vectors x and y must be the same length'
end

if (isa(labels,'string'))
  labelarray = repmat({labels},[length(xvec),1]);
elseif (iscellstr(labels))
  labelarray = labels;
  if (length(labelarray) ~= length(xvec))
    error 'number of labels much match the length of the x and y vectors'
  end
else
  error 'labels must be either a single string or a cell array of strings.'
end

for (i=1:numel(xvec))
  htext('position',[xvec(i),yvec(i)],'string',labelarray{i},varargin{:});
end

