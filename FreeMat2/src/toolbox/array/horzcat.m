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
%!
function y = horzcat(varargin)
  y = [varargin{:}];
