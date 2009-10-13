%!
%@Module VERTCAT Horizontal Array Concatenation
%@@Section CLASS
%@@Usage
%This function concatenates arrays vertically (along the row
%dimension).  The syntax for its use is
%@[
%   d = vertcat(a,b,c)
%@]
%which is equivalent to the statement @|d = [a;b;c]|.
%@@Tests
%@$exact#y1=vertcat(x1,x2)
%!

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
