%!
%@Module LENGTH Length of an Array
%@@Section INSPECTION
%@@Usage
%Returns the length of an array @|x|.  The syntax for its use
%is 
%@[
%   y = length(x)
%@]
%and is defined as the maximum length of @|x| along any of its
%dimensions, i.e., @|max(size(x))|.  If you want to determine the
%number of elements in @|x|, use the @|numel| function instead.
%@@Example
%For a @|4 x 4 x 3| matrix, the length is @|4|, not @|48|, as you 
%might expect.
%@<
%x = rand(4,4,3);
%length(x)
%@>
%!

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function len = length(x)
if (isempty(x))
  len = 0;
else
  len = max(size(x));
end
  
