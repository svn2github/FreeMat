%!
%@@Module TRIL Lower Triangular Matrix Function
%@@Section ARRAY
%@@Usage
% 
% Returns the lower triangular matrix of a square matrix. The general
% syntax for its use is
%@[
%   y = tril(x)
%@]
% where |x| is a square matrix. This returns the lower triangular matrix
% (i.e.: all cells on or above the diagonal are set to 0). You can also
% specify a different diagonal using the alternate form
%@[
%   y = tril(x,n)
%@]
% where n is the diagonal offset. In this mode, the diagonal specified 
% is not set to zero in the returned matrix (e.g.: tril(x) and tril(x,-1))
% will return the same value.
%!

% Copyright (c) 2008 Samit Basu
% Licensed under the GPL

function ret = tril2(x,n)
   
   if nargin == 1
      for i = 1:size(x,1)
         for j = i+1:size(x,2)
            x(i,j) = 0;
         end
      end
  
   elseif nargin == 2     

      if n < 0
         t1 = zeros(size(x));
         for i = 1 + abs(n):size(x,1)
            for j = 1:i+n
               t1(i,j) = 1;
            end
         end
      elseif n > 0
         t1 = ones(size(x));
         for i = 1:size(x,1)-n
            for j = i+n+1:size(x,2)
               t1(i,j) = 0;
            end
         end
      elseif n == 0
          for i = 1:size(x,1)
            for j = i+1:size(x,2)
                x(i,j) = 0;
            end
        end
      end
      x = x.*t1;
   end
      ret = x;
end

