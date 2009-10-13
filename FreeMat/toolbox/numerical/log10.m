% LOG10 LOG10 Base-10 Logarithm Function
% 
% Usage
% 
% Computes the log10 function for its argument.  The general
% syntax for its use is
% 
%   y = log10(x)
% 
% where x is an n-dimensional array of numerical type.
% Integer types are promoted to the double type prior to
% calculation of the log10 function.  Output y is of the
% same size as the input x. For strictly positive, real inputs, 
% the output type is the same as the input.
% For negative and complex arguments, the output is complex.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = log10(x)
  y = log(x)/log(10.0);

