%!
%@Module LOG10 Base-10 Logarithm Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the @|log10| function for its argument.  The general
%syntax for its use is
%@[
%  y = log10(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%Integer types are promoted to the @|double| type prior to
%calculation of the @|log10| function.  Output @|y| is of the
%same size as the input @|x|. For strictly positive, real inputs, 
%the output type is the same as the input.
%For negative and complex arguments, the output is complex.
%@@Example
%The following piece of code plots the real-valued @|log10|
%function over the interval @|[1,100]|:
%@<
%x = linspace(1,100);
%plot(x,log10(x))
%xlabel('x');
%ylabel('log10(x)');
%mprint('log10plot');
%@>
%@figure log10plot
%!
function y = log10(x)
  if (isa(x,'float') | isa(x,'complex'))
    y = log(x)/log(10.0f);
  else
    y = log(x)/log(10.0);
  end

