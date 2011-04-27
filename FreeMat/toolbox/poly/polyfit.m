% POLYFIT Fit Polynomial To Data
% 
% Usage
% 
% The polyfit routine has the following syntax
% 
%   p = polyfit(x,y,n)
% 
% where x and y are vectors of the same size, and
% n is the degree of the approximating polynomial.  
% The resulting vector p forms the coefficients of
% the optimal polynomial (in descending degree) that fit
% y with x.  

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function p = polyfit(x,y,n)
x = x(:);
y = y(:);
if (size(x,1) ~= size(y,1))
  error('size mismatch on arguments to polyfit - they should be the same size');
end
if (n < 0 | n > 200)
  error('invalid polynomial order in polyfit - must be between [1,200] inclusive');
end
A = zeros(size(x,1),n+1);
xp = x*0+1;
for i=(n+1):-1:1
  A(:,i) = xp;
  xp = xp.*x;
end
p = (pinv(A)*y)';
