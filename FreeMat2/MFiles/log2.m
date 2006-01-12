%!
%@Module LOG2 Base-2 Logarithm Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the @|log2| function for its argument.  The general
%syntax for its use is
%@[
%  y = log2(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%Integer types are promoted to the @|double| type prior to
%calculation of the @|log2| function.  Output @|y| is of the
%same size as the input @|x|. For strictly positive, real inputs, 
%the output type is the same as the input.
%For negative and complex arguments, the output is complex.
%@@Example
%The following piece of code plots the real-valued @|log2|
%function over the interval @|[1,100]|:
%@<
%x = linspace(1,100);
%plot(x,log2(x))
%xlabel('x');
%ylabel('log2(x)');
%mprintplot('log2plot');
%@>
%@figure log2plot
%!
function y = log2(x)
  if (isa(x,'float') | isa(x,'complex'))
    y = log(x)/log(2.0f);
  else
    y = log(x)/log(2.0);
  end

