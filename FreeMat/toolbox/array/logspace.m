% LOGSPACE LOGSPACE Logarithmically Spaced Vector
% 
% Usage
% 
% Generates a row vector with the specified number number of elements,
% with entries logarithmically spaced between two specified endpoints.
% The syntax for its use is either
% 
%     y = logspace(a,b,count)
% 
%  or, for a default count = 50,
% 
%     y = logspace(a,b)
% 
%  A third special use is when
% 
%     y = logspace(a,pi)
% 
% where it generates points between 10^a and pi
% 
% Contributed by Paulo Xavier Candeias under GPL.
function y = logspace(a,b,count)
   if (nargin < 2) | (nargout > 1)
      error('wrong use (see help logspace)')
   elseif (nargin < 3)
      count = 50;
   end
   if (b == pi)
      b = log10(b);
   end
   y = 10.^(a+(b-a)/(count-1)*(0:count-1));
   if isinf(y(end))
      warning('logspace: upper limit too large')
   end
