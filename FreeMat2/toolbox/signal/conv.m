%!
%@Module CONV Convolution Function
%@@Section SIGNAL
%@@Usage
%The @|conv| function performs a one-dimensional convolution of two
%vector arguments.  The syntax for its use is
%@[
%     z = conv(x,y)
%@]
%where @|x| and @|y| are vectors.  The output is of length @|nx + ny -1|.
%The @|conv| function calls @|conv2| to do the calculation.  See its
%help for more details.
%@@Tests
%@{"a=[0.5,1];b=[1,2,3];y=conv(a,b)","[0.5,2.0,3.5,3.0]","close"}
%@{"a=[1];b=[1,2,3];y=conv(a,b)","[1,2,3]","close"}
%!
% Copyright (c) 2002-2006 Samit Basu

function z = conv(x,y)
% is x a column vector
xiscol = size(x,1) > size(x,2);
yiscol = size(y,1) > size(y,2);
if (xiscol | yiscol)
  z = conv2(x(:),y(:));
else
  z = conv2(x(:).',y(:).');
end

