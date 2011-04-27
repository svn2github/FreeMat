% INLINE Construct Inline Function
% 
% Usage
% 
% Constructs an inline function object.  The syntax for its use is
% either
% 
%    y = inline(expr)
% 
% which uses the symvar function to identify the variables in the
% expression, or the explicit form
% 
%    y = inline(expr,var1,var2,...,varn)
% 
% where the variables are explicitly given.  Note that inline functions
% are only partially supported in FreeMat.  If you need features of the
% inline function that are not currently implemented, please file a
% feature request at the FreeMat website.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = inline(expr,varargin)
  % Create a default structure - we store the original expression
  % and the expression that assigns the arguments
  default.expr = '';
  default.assign_args = '';
  default.args = {};
  if (nargin == 0)
    y = class(default,'inline');
  elseif ((nargin == 1) && isa(expr,'inline'))
    y = expr;
  else
    if (nargin == 1)
      varargin = symvar(expr);
    end
    assign_args = '';
    for i=1:length(varargin)
      if (~ischar(varargin{i}))
	error(sprintf('argument %d to inline constructor is not a string',i));
      end
      assign_args = sprintf('%s %s = args{%d};',assign_args,varargin{i},i);
    end
    default.expr = expr;
    default.assign_args = assign_args;
    default.args = varargin;
    y = class(default,'inline');
  end
