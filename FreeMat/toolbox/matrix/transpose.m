% TRANSPOSE Overloaded Transpose Operator
% 
% Usage
% 
% This is a method that is invoked when a variable has the
% transpose operator method applied, and is invoked
% when you call
% 
%    c = transpose(a)
% 
% or
% 
% /  c = a.'
% 


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = transpose(x)
    y = x.';
