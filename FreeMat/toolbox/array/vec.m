% VEC Reshape to a Vector
% 
% Usage
% 
% Reshapes an n-dimensional array into a column vector.  The general
% syntax for its use is
% 
%    y = vec(x)
% 
% where x is an n-dimensional array (not necessarily numeric).  This
% function is equivalent to the expression y = x(:).
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = vec(x)
  y = x(:);
  
