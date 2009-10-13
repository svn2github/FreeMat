%!
%@Module HORZCAT Horizontal Array Concatenation
%@@Section CLASS
%@@Usage
%This function concatenates arrays horizontally (along the column
%dimension).  The syntax for its use is
%@[
%   d = horzcat(a,b,c)
%@]
%which is equivalent to the statement @|d = [a,b,c]|.
%@@Tests
%@$exact#y1=horzcat(x1,x2)
%@$exact#y1=horzcat(x1)
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = horzcat(varargin)
  y = [varargin{:}];
