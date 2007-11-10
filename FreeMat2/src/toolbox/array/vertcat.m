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
%!
function y = vertcat(varargin)
  if (nargin == 0)
    y = [];
  else
    y = varargin{1};
    for i=2:nargin
      y = [y;varargin{i}];
    end
  end
