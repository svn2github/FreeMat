% MPOWER Overloaded Matrix Power Operator
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
function y = mpower(a,b)
  y = a^b;
  
