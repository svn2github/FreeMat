% HOLD HOLD Plot Hold Toggle Function
% 
% Usage
% 
% Toggles the hold state on the currently active plot.  The
% general syntax for its use is
% 
%    hold(state)
% 
% where state is either
% 
%    hold('on')
% 
% to turn hold on, or
% 
%    hold('off')
% 
% to turn hold off. If you specify no argument then
% hold toggles the state of the hold:
% 
%    hold
% 
% You can also specify a particular axis to the hold command
% 
%    hold(handle,...)
% 
% where handle is the handle for a particular axis.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function h = hold(varargin)
if (nargin == 0)
     hold_toggle(gca);
elseif (strcmp(varargin{1},'on'))
     hold_on(gca);
elseif (strcmp(varargin{1},'off'))
     hold_off(gca);
elseif (ishandle(varargin{1}))
   if (nargin == 1)
     hold_toggle(varargin{1})
   elseif (strcmp(varargin{2},'on'))
     hold_on(varargin{1});
   elseif (strcmp(varargin{2},'off'))
     hold_off(varargin{1});
   else
     error('Unrecognized form of hold');
   end
else
   error('Unrecognized arguments to hold');
end

function hold_off(handle)
  set(gca,'nextplot','replace');

function hold_on(handle)
  set(gcf,'nextplot','add');
  set(gca,'nextplot','add');

function hold_toggle(handle);
  if (strcmp(get(gca,'nextplot'),'replace'))
    set(gca,'nextplot','add');
  else
    set(gca,'nextplot','replace');
  end

