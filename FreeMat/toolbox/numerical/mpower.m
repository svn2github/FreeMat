% MPOWER MPOWER Overloaded Matrix Power Operator
% 
% Usage
% 
% This is a method that is invoked when one variable is raised
% to another variable using the matrix power operator, and
% is invoked when you call
% 
%   c = mpower(a,b)
% 
% or
% 
%   c = a^b
% 
% MPOWER MPOWER Matrix Power Function
% 
% Usage
% 
% Computes the matrix power operator for two arrays.  It is an
% M-file version of the ^ operator.  The syntax for its use is
% 
%    y = mpower(a,b)
% 
% where y=a^b.  See the matrixpower documentation for more
% details on what this function actually does.
function y = mpower(a,b)
  y = a^b;
  
