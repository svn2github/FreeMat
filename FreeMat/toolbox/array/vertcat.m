% VERTCAT VERTCAT Overloaded Vertical Concatenation
% 
% Usage
% 
% This is a method for a class that is invoked to concatenate two or more
% variables of the same class type together.  Besides being called when
% you invoke
% 
%    c = vertcat(a,b,c)
% 
% when a is a class, it is also called for
% 
%    c = [a;b;c]
% 
% when one of the variables is a class.  The exact meaning of 
% vertical concatenation depends on the class you have designed.
% VERTCAT VERTCAT Horizontal Array Concatenation
% 
% Usage
% 
% This function concatenates arrays vertically (along the row
% dimension).  The syntax for its use is
% 
%    d = vertcat(a,b,c)
% 
% which is equivalent to the statement d = [a;b;c].

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = vertcat(varargin)
  if (nargin == 0)
    y = [];
  else
    y = varargin{1};
    for i=2:nargin
      y = [y;varargin{i}];
    end
  end
