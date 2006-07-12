%!
%@Module STD Standard Deviation Function
%@@Section ELEMENTARY
%@@Usage
%Computes the standard deviation of an array along a given dimension.  
%The general syntax for its use is
%@[
%  y = std(x,d)
%@]
%where @|x| is an @|n|-dimensions array of numerical type.
%The output is of the same numerical type as the input.  The argument
%@|d| is optional, and denotes the dimension along which to take
%the variance.  The output @|y| is the same size as @|x|, except
%that it is singular along the mean direction.  So, for example,
%if @|x| is a @|3 x 3 x 4| array, and we compute the mean along
%dimension @|d=2|, then the output is of size @|3 x 1 x 4|.
%@@Example
%The following piece of code demonstrates various uses of the @|std|
%function
%@<
%A = [5,1,3;3,2,1;0,3,1]
%@>
%We start by calling @|std| without a dimension argument, in which 
%case it defaults to the first nonsingular dimension (in this case, 
%along the columns or @|d = 1|).
%@<
%std(A)
%@>
%Next, we take the variance along the rows.
%@<
%std(A,2)
%@>
%@@Tests
%@{"A = [5,1,3;3,2,1;0,3,1];y = std(A)","[2.51661147842358  1.0000000000000000  1.15470053837925]","close"}
%@{"A = [5,1,3;3,2,1;0,3,1];y = std(A,2)","[2;1;1.52752523165195]","close"}
%!
function y = std(varargin)
  y = sqrt(var(varargin{:}));
