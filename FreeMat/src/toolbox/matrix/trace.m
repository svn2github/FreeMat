%!
%@Module TRACE Sum Diagonal Elements of an Array
%@@Section ARRAY
%@@Usage
% 
% Returns the sum of the diagonal elements of a square matrix. The general
% syntax for its use is
%@[
%   y = trace(x)
%@]
% where x is a square matrix.
%!

% Copyright (c) 2008 Samit Basu
% Licensed under the GPL

function ret = trace(x)
   
   [m n] = size(x);
   
   if m ~= n
        error('Matrix must be square!\n');
   end
   
   ret = sum(diag(x));
   
   end

