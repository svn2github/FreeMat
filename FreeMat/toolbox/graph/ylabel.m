% YLABEL Plot Y-axis Label Function
% 
% Usage
% 
% This command adds a label to the y-axis of the plot.  The general syntax
% for its use is
% 
%   ylabel('label')
% 
% or in the alternate form
% 
%   ylabel 'label'
% 
% or simply
% 
%   ylabel label
% 
% You can also specify properties for that label using the syntax
% 
%   ylabel('label',properties...) 
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function h = ylabel(varargin)
  if (nargin < 1) 
    error('ylabel needs at least one argument')
  end
  if (nargout > 1) 
    error('ylabel outputs at most one argument')
  end
  h1 = htext('string',varargin{1},'parent',gca,'horiz','center','vert','middle','autoparent','off',varargin{2:end});
  set(gca,'ylabel',h1);
  if (nargout == 1) 
    h = h1;
  end
