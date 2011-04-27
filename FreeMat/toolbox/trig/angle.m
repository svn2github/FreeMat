% ANGLE Phase Angle Function
% 
% Usage
% 
% Compute the phase angle in radians of a complex matrix.  The general
% syntax for its use is
% 
%   p = angle(c)
% 
% where c is an n-dimensional array of numerical type.

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function p = angle(c)

p = atan2(imag(c), real(c));

