% POWER POWER Overloaded Power Operator
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
% POWER POWER Element-wise Power Function
% 
% Usage
% 
% Computes the element-wise power operator for two arrays.  It is an
% M-file version of the .^ operator.  The syntax for its use is
% 
%    y = power(a,b)
% 
% where y=a.^b.  See the dotpower documentation for more
% details on what this function actually does.
function y = power(a,b)
  y = a.^b;
  
