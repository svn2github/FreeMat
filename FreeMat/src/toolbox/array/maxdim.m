%!
%@Module MAXDIM Maximum Dimension in Array
%@@Section INSPECTION
%@@Usage
%The @|maxdim| function returns the lowest order dimension
%along which an array is largest.  The general syntax for its
%use is
%@[
%  n = maxdim(x)
%@]
%and is equivalent to min(find(size(x) == max(size(x)))).
%@@Tests
%@$exact#y1=maxdim(x1)
%!

function n = maxdim(x);
  n = min(find(size(x) == max(size(x))));
