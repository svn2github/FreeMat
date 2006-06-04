%!
%@Module SEMILOGY Semilog Y Axis Plot Function
%@@Section HANDLE
%@@Usage
%This command has the exact same syntax as the @|plot| command:
%@[
%  semilogy(<data 1>,{linespec 1},<data 2>,{linespec 2}...,properties...)
%@]
%in fact, it is a simple wrapper around @|plot| that sets the
%y axis to have a logarithmic scale.
%@@Example
%Here is an example of an exponential signal plotted first on a linear
%plot:
%@<
%x = linspace(0,2);
%y = 10.0.^x;
%plot(x,y,'r-');
%mprint semilogy1
%@>
%@figure semilogy1
%and now with a logarithmic y axis
%@<
%semilogy(x,y,'r-');
%mprint semilogy2
%@>
%@figure semilogy2
%!
function ohandle = semilogy(varargin)
    handle = plot(varargin{:});
    set(gca,'yscale','log');
    if (nargout > 0)
        ohandle = handle;
    end
    
