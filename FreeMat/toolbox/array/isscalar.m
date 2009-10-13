% ISSCALAR ISSCALAR Test For Scalar
% 
% Usage
% 
% The syntax for isscalar is
% 
%    x = isscalar(y)
% 
% and it returns a logical 1 if the argument is a scalar,
%  and a logical 0 otherwise.
%   M version contributor: M.W. Vogel 01-22-06
% Licensed under the GPL
function v = isscalar(a)
  v = (numel(a) == 1);
  
