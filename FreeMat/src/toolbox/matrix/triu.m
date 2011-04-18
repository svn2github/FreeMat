%!
%@Module TRIU Upper Triangular Matrix Function
%@@Section ARRAY
%@@Usage
% 
% Returns the upper triangular matrix of a square matrix. The general
% syntax for its use is
%@[
%   y = triu(x)
%@]
% where x is a square matrix. This returns the upper triangular matrix
% (i.e.: all elements on or below the diagonal are set to 0). You can also
% specify a different diagonal using the alternate form
%@[
%   y = triu(x,n)
%@]
% where n is the diagonal offset. In this mode, the diagonal specified 
% is not set to zero in the returned matrix (e.g.: tril(x) and tril(x,1))
% will return the same value.
%!

% Copyright (c) 2008 Samit Basu
% Licensed under the GPL

function ret = triu(x,n)
   
   [m n] = size(x);
   
   if m ~= n
        error('Matrix must be square!\n');
   end
   
   if nargin == 1
      t1 = zeros(size(x));
      for i = 1:size(x,1)
         for j = i:size(x,2)
            t1(i,j) = 1;
         end
      end
  
   elseif nargin == 2     

      if n < 0
         t1 = ones(size(x));
         for i = 2 + abs(n):size(x,1)
            for j = 1:i-abs(n)-1
               t1(i,j) = 0;
            end
         end
      elseif n > 0
         t1 = zeros(size(x));
         for i = 1:size(x,1)-n
            for j = i+n:size(x,2)
               t1(i,j) = 1;
            end
         end
      elseif n == 0
          t1 = zeros(size(x));
            for i = 1:size(x,1)
                for j = i:size(x,2)
                    t1(i,j) = 1;
                end
            end
      end
   end
   x = x.*t1;
   ret = x;
end

