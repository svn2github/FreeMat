%!
%@Module LOGLOG Log-Log Plot Function
%@@Section HANDLE
%@@Usage
%This command has the exact same syntax as the @|plot| command:
%@[
%  loglog(<data 1>,{linespec 1},<data 2>,{linespec 2}...,properties...)
%@]
%in fact, it is a simple wrapper around @|plot| that sets the
%x and y axis to have a logarithmic scale.
%@@Example
%Here is an example of a doubly exponential signal plotted first on a linear
%plot:
%@<
%x = linspace(1,100);
%y = x;
%plot(x,y,'r-');
%mprint loglog1
%@>
%@figure loglog1
%and now on a log-log plot
%@<
%loglog(x,y,'r-');
%mprint loglog2
%@>
%@figure loglog2
%!
function ohandle = loglog(varargin)
    handle = plot(varargin{:});
    set(gca,'xscale','log');
    set(gca,'yscale','log');
    if (nargout > 0)
        ohandle = handle;
    end
    
