% POWER Overloaded Power Operator
% 
% Usage
% 
% This is a method that is invoked when one variable is raised
% to another variable using the dot-power operator, and is
% invoked when you call
% 
%    c = power(a,b)
% 
% or
% 
%    c = a.^b
% 
function y = power(a,b)
  y = a.^b;
  
