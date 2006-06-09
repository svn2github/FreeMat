%!
%@Module SEMILOGX Semilog X Axis Plot Function
%@@Section HANDLE
%@@Usage
%This command has the exact same syntax as the @|plot| command:
%@[
%  semilogx(<data 1>,{linespec 1},<data 2>,{linespec 2}...,properties...)
%@]
%in fact, it is a simple wrapper around @|plot| that sets the
%x axis to have a logarithmic scale.
%@@Example
%Here is an example of an exponential signal plotted first on a linear
%plot:
%@<
%y = linspace(0,2);
%x = (10).^y
%plot(x,y,'r-');
%mprint semilogx1
%@>
%@figure semilogx1
%and now with a logarithmic x axis
%@<
%semilogx(x,y,'r-');
%mprint semilogx2
%@>
%@figure semilogx2
%!
function ohandle = semilogx(varargin)
    handle = plot(varargin{:});
    set(gca,'xscale','log');
    if (nargout > 0)
        ohandle = handle;
    end
    
