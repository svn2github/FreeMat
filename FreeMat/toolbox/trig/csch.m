% CSCH CSCH Hyperbolic Cosecant Function
% 
% Usage
% 
% Computes the hyperbolic cosecant of the argument.
% The syntax for its use is
% 
%    y = csch(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = csch(x)
  y = 1.0 ./ sinh(x);
  
