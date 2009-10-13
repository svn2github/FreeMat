%!
%@Module CIRCSHIFT Circularly Shift an Array
%@@Section ARRAY
%@@USAGE
%Applies a circular shift along each dimension of a given array.  The
%syntax for its use is
%@[
%   y = circshift(x,shiftvec)
%@]
%where @|x| is an n-dimensional array, and @|shiftvec| is a vector of
%integers, each of which specify how much to shift @|x| along the
%corresponding dimension.  
%@@Example
%The following examples show some uses of @|circshift| on N-dimensional
%arrays.
%@<
%x = int32(rand(4,5)*10)
%circshift(x,[1,0])
%circshift(x,[0,-1])
%circshift(x,[2,2])
%x = int32(rand(4,5,3)*10)
%circshift(x,[1,0,0])
%circshift(x,[0,-1,0])
%circshift(x,[0,0,-1])
%circshift(x,[2,-3,1])
%@>
%@@Tests
%@$exact#y1=circshift(x1,[1,0])
%@$exact#y1=circshift(x1,[0,-1])
%@$exact#y1=circshift(x1,[2,2])
%@$exact#y1=circshift(x1,[1,0,0])
%@$exact#y1=circshift(x1,[0,-1,0])
%@$exact#y1=circshift(x1,[0,0,-1])
%@$exact#y1=circshift(x1,[2,-3,1])
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function x = circshift(y,shiftvec)
  szey = size(y);
  ndim = prod(size(szey));
  shiftvec = shiftvec(:);
  shiftlen = prod(size(shiftvec));
  d = {};
  for k=1:ndim
    if (k<=shiftlen)
      shift = shiftvec(k);
    else
      shift = 0;
    end
    d{k} = mod((1:szey(k)) - 1 - shift,szey(k)) + 1;
  end;
  x = y(d{:});
