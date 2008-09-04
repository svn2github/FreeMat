%!
%@Module RANDPERM Random permutation
%@@Section RANDOM
%@@USAGE
%@|randperm| generates random permutation with syntax
%@[
%   y = randperm(n)
%@]
%@|y| is a random permutation of integers from 1 to @|n|.
%RANDPERM calls RAND and changes its state.
%@@Example
%@<
%y = randperm(10)
%@>
%!

% Copyright (c) 2008 Samit Basu
% Licensed under the GPL

function result = randperm(n)
% %     Slow version
%     m = floor(n);
%     result = 1:m;
%     for ii = 1:m
%         jj = round(rand(1,1)*(m-1))+1;
%         temp = result(ii);
%         result(ii) = result(jj);
%         result(jj) = temp;
%     end

%   This is much faster
    [A, result] = sort(rand(1,n));
end
