%!
%@Module NDIMS Number of Dimensions in Array
%@@Section Array
%@@Usage
%The @|ndims| function returns the number of dimensions
%allocated in an array.  The general syntax for its
%use is
%@[
%  n = ndims(x)
%@]
%and is equivalent to @|length(size(x))|.
%!

% Copyright (c) 2002-2006 Samit Basu

function n = ndims(x)
n = length(size(x));
