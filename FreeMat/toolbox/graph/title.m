% TITLE Plot Title Function
% 
% Usage
% 
% This command adds a title to the plot.  The general syntax
% for its use is
% 
%   title('label')
% 
% or in the alternate form
% 
%   title 'label'
% 
% or simply
% 
%   title label
% 
% Here label is a string variable.  You can also specify 
% properties for the label, and a handle to serve as a target
% for the operation
% 
%   title(handle,'label',properties...)
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function o = title(varargin)
  % Check for an axes handle
  if (nargin>=2)
    if (isnumeric(varargin{1}) && (length(varargin{1})==1) && ...
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
  if (nargout > 1) 
    error('title outputs at most one argument')
  end
  mtext = varargin{1};
  varargin(1) = [];
  nargin = nargin - 1;
  o1 = htext('string',mtext,'horiz','center','vert','top','position',[0.5,1,0],'parent',handle,'autoparent','off',varargin{:});
  set(handle,'title',o1);
  axes(saveca);
  if (nargout == 1) 
    o = o1;
  end
