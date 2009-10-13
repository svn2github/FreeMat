% HORZCAT HORZCAT Overloaded Horizontal Concatenation
% 
% Usage
% 
% This is a method for a class that is invoked to concatenate two or more
% variables of the same class type together.  Besides being called
% when you invoke
% 
%    c = horzcat(a,b,c)
% 
% when a is a class, it is also called for 
% 
%    c = [a,b,c]
% 
% when one of these variables is a class.  The exact meaning of
% horizontal concatenation depends on the class you have designed.
% HORZCAT HORZCAT Horizontal Array Concatenation
% 
% Usage
% 
% This function concatenates arrays horizontally (along the column
% dimension).  The syntax for its use is
% 
%    d = horzcat(a,b,c)
% 
% which is equivalent to the statement d = [a,b,c].

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = horzcat(varargin)
  y = [varargin{:}];
