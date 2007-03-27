%!
%@Module ANY Any True Function
%@@Section ELEMENTARY
%@@Usage
%Reduces a logical array along a given dimension by testing for any
%logical 1s.  The general 
%syntax for its use is
%@[
%  y = any(x,d)
%@]
%where @|x| is an @|n|-dimensions array of @|logical| type.
%The output is of @|logical| type.  The argument @|d| is 
%optional, and denotes the dimension along which to operate.
%The output @|y| is the same size as @|x|, except that it is 
%singular along the operated direction.  So, for example,
%if @|x| is a @|3 x 3 x 4| array, and we @|any| operation along
%dimension @|d=2|, then the output is of size @|3 x 1 x 4|.
%@@Function Internals
%The output is computed via
%\[
%y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
%\max_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
%\]
%If @|d| is omitted, then the summation is taken along the 
%first non-singleton dimension of @|x|. 
%@@Example
%The following piece of code demonstrates various uses of the summation
%function
%@<
%A = [1,0,0;1,0,0;0,0,1]
%@>
%We start by calling @|any| without a dimension argument, in which 
%case it defaults to the first nonsingular dimension (in this case, 
%along the columns or @|d = 1|).
%@<
%any(A)
%@>
%Next, we apply the @|any| operation along the rows.
%@<
%any(A,2)
%@>
%@@Tests
%@$"y=any([1,0,0;1,0,0;0,0,1],2)","[1;1;1]","exact"
%!

% Copyright (c) 2002-2006 Samit Basu

function y = any(A,dim)
  if (nargin == 0)
    error 'any function requires at least one argument'
  end
  if (nargin == 1)
    y = max(logical(A(:)));
  else
    y = max(logical(A),[],dim);
  end
