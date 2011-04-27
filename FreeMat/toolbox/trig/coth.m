% COTH Hyperbolic Cotangent Function
% 
% Usage
% 
% Computes the hyperbolic cotangent of the argument.
% The syntax for its use is
% 
%    y = coth(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = coth(x)
  y = 1.0 ./ tanh(x);
  
