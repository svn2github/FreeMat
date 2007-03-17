%!
%@Module SQUEEZE Remove Singleton Dimensions of an Array
%@@Section ARRAY
%@@Usage
%This function removes the singleton dimensions of an array.  The
%syntax for its use is
%@[
%   y = squeeze(x)
%@]
%where @|x| is a multidimensional array.  Generally speaking, if
%@|x| is of size @|d1 x 1 x d2 x ...|, then @|squeeze(x)| is of
%size @|d1 x d2 x ...|, i.e., each dimension of @|x| that was
%singular (size 1) is squeezed out.
%@@Example
%Here is a many dimensioned, ungainly array, both before and 
%after squeezing;
%@<
%x = zeros(1,4,3,1,1,2);
%size(x)
%y = squeeze(x);
%size(y)
%@>
%!
% Copyright 2004-2005, 2006 Brian Yanoff, Dirk Beque
function B = squeeze(A)
  sz = size(A);
  sz(find(sz==1)) = [];
  if length(sz)==1
    sz = [1, sz];
  end
  if length(sz)==0
    sz = [1,1];
  end
  B = reshape(A,sz);
  

  
