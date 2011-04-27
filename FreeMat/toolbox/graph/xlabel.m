% XLABEL Plot X-axis Label Function
% 
% Usage
% 
% This command adds a label to the x-axis of the plot.  The general syntax
% for its use is
% 
%   xlabel('label')
% 
% or in the alternate form
% 
%   xlabel 'label'
% 
% or simply
% 
%   xlabel label
% 
% Here label is a string variable.  You can also specify properties
% for that label using the syntax
% 
%   xlabel('label',properties...) 
% 

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function h = xlabel(varargin)
  if (nargin < 1) 
    error('xlabel needs at least one argument')
  end
  if (nargout > 1) 
    error('xlabel outputs at most one argument')
  end
  h1 = htext('string',varargin{1},'parent',gca,'horiz','center','vert','middle','autoparent','off',varargin{2:end});
  set(gca,'xlabel',h1);
  if (nargout == 1) 
    h = h1;
  end
