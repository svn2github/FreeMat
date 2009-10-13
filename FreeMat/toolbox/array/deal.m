% DEAL DEAL Multiple Simultaneous Assignments
% 
% Usage
% 
% When making a function call, it is possible to assign
% multiple outputs in a single call, (see, e.g., max for
% an example).  The deal call allows you to do the 
% same thing with a simple assignment.  The syntax for its
% use is
% 
%    [a,b,c,...] = deal(expr)
% 
% where expr is an expression with multiple values.  The simplest
% example is where expr is the dereference of a cell array, e.g.
% expr <-- A{:}.  In this case, the deal call is equivalent
% to
% 
%    a = A{1}; b = A{2}; C = A{3}; 
% 
% Other expressions which are multivalued are structure arrays with
% multiple entries (non-scalar), where field dereferencing has been
% applied.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function varargout = deal(varargin)
  if (nargin ~= nargout) && (nargin ~= 1)
    error('number of outputs must match number of inputs');
  end
  if (nargin == 1)
    varargout = varargin(ones(1,max(1,nargout)));
  else
    varargout = varargin;
  end

    
