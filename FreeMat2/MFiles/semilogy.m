%!
%@Module SEMILOGY Semilog Y Axis Plot Function
%@@Section HANDLE
%@@Usage
%!
function ohandle = semilogy(varargin)
    ohandle = plot(varargin{:});
    set(gca,'yscale','log');
    
